--TEST--
curl_exec() integrates with io_hooks scheduler
--EXTENSIONS--
curl
--FILE--
<?php

include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

function go(callable $fn): void
{
    global $scheduler;
    $scheduler->go($fn);
}

$scheduler->spawn(function () {
    $server = stream_socket_server('tcp://127.0.0.1:0');
    $addr = stream_socket_get_name($server, false);

    go(function () use ($server) {
        $conn = stream_socket_accept($server, 5);
        $request = '';
        while (!str_ends_with($request, "\r\n\r\n")) {
            $chunk = fread($conn, 1024);
            if ($chunk === false || $chunk === '') break;
            $request .= $chunk;
        }
        fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Length: 12\r\n\r\nHello, curl!");
        fclose($conn);
    });

    go(function () use ($addr) {
        $ch = curl_init("http://$addr/");
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_TIMEOUT, 5);
        $body = curl_exec($ch);
        $errno = curl_errno($ch);
        if ($errno === 0) {
            echo "OK: $body\n";
        } else {
            echo "curl error $errno: " . curl_error($ch) . "\n";
        }
    });
});
$scheduler->loop();

?>
--EXPECT--
OK: Hello, curl!
