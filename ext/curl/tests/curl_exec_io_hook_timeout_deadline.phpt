--TEST--
curl_exec() under io_hooks: libcurl's timeout is a deadline, not re-armed after socket activity
--EXTENSIONS--
curl
--FILE--
<?php

include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

class TimerScheduler extends Scheduler
{
    public int|float $latestDeadline = 0;   /* hrtime() is a float on 32-bit */

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof Io\Operation\Any) {
            foreach ($op->getOperations() as $member) {
                if ($member instanceof Io\Operation\Timer) {
                    $now = hrtime(true);
                    $t = $member->getTimeout();
                    $this->latestDeadline = max($this->latestDeadline, $now + $t->seconds * 1_000_000_000 + $t->nanoseconds);
                }
            }
        }
        return parent::run($op);
    }
}

$scheduler = new TimerScheduler();
Io\Hooks\set_hooks($scheduler);

[$ctl_server, $ctl_client] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);

$scheduler->spawn(function () use ($server, $ctl_server) {
    $conn = stream_socket_accept($server, 5);
    $request = '';
    while (!str_ends_with($request, "\r\n\r\n")) {
        $chunk = fread($conn, 1024);
        if ($chunk === false || $chunk === '') break;
        $request .= $chunk;
    }
    usleep(300000);
    fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Length: 20\r\n\r\n0123456789");
    fread($ctl_server, 1);
    fclose($conn);
});

$scheduler->spawn(function () use ($scheduler, $addr, $ctl_client) {
    $ch = curl_init("http://$addr/");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT_MS, 1000);
    $start = hrtime(true);
    var_dump(curl_exec($ch), curl_errno($ch));
    $overshoot_ms = ($scheduler->latestDeadline - $start) / 1e6 - 1000;
    echo $overshoot_ms < 50 ? "deadline kept\n" : "deadline moved by {$overshoot_ms}ms\n";
    fwrite($ctl_client, "x");
});

$scheduler->loop();
?>
--EXPECT--
bool(false)
int(28)
deadline kept
