--TEST--
curl_multi_select() under IO hooks waits with an Any operation
--EXTENSIONS--
curl
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

final class Tracing extends Scheduler
{
    public int $anys = 0;
    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\Any) {
            $this->anys++;
        }
        return parent::run($op);
    }
}

$scheduler = new Tracing();
Io\Hooks\set_hooks($scheduler);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);

$scheduler->spawn(function () use ($server) {
    $conn = stream_socket_accept($server, 5);
    $request = '';
    while (!str_ends_with($request, "\r\n\r\n")) {
        $chunk = fread($conn, 1024);
        if ($chunk === false || $chunk === '') break;
        $request .= $chunk;
    }
    usleep(20000);
    fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Length: 5\r\n\r\nmulti");
    fclose($conn);
});

$scheduler->spawn(function () use ($addr) {
    $mh = curl_multi_init();
    $ch = curl_init("http://$addr/");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_multi_add_handle($mh, $ch);
    $selects = 0;
    do {
        $status = curl_multi_exec($mh, $active);
        if ($active) {
            $n = curl_multi_select($mh, 1.0);
            var_dump($n >= 0);
            $selects++;
        }
    } while ($active && $status == CURLM_OK);
    var_dump(curl_multi_getcontent($ch));
    curl_multi_remove_handle($mh, $ch);
    var_dump($selects >= 1);
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
var_dump($scheduler->anys >= 1);
?>
--EXPECTF--
%A
string(5) "multi"
bool(true)
bool(true)
