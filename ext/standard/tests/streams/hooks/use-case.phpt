--TEST--
Stream hook: use case
--FILE--
<?php

include __DIR__ . '/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

function go(callable $fn): void
{
    global $scheduler;
    $scheduler->go($fn);
}

$scheduler->spawn(function () {
    $server = stream_socket_server('tcp://localhost:0');
    $socket_name = stream_socket_get_name($server, false);
    if (!preg_match('/:(\d+)$/', $socket_name, $m)) {
        die("Could not extract port from '$socket_name'");
    }
    $port = $m[1];

    go(function () use ($server) {
        $client = stream_socket_accept($server);
        go(function () use ($client) {
            $headers = [];
            while (!feof($client)) {
                $line = fgets($client);
                if ($line === false) {
                    break;
                }
                if ($line === "\r\n") {
                    break;
                }
                $headers[] = $line;
            }
            foreach ($headers as $header) {
                echo "> " . trim($header) . "\n";
            }
            fwrite($client, "HTTP/1.0 200 OK\r\n");
            fwrite($client, "\r\n");
            fwrite($client, "Hello world!\n");
        });
    });

    go(function () use ($port) {
        $fd = stream_socket_client("tcp://localhost:$port");
        fwrite($fd, "GET / HTTP/1.0\r\n");
        fwrite($fd, "Host: localhost\r\n");
        fwrite($fd, "\r\n");
        /* Not feof(): on Windows the server's close lingers as a Poll op, so
         * the client may read the last line before the close and see EOF only
         * from the next read */
        while (($line = fgets($fd)) !== false) {
            echo trim("< " . $line) . "\n";
        }
    });
});
$scheduler->loop();

?>
--EXPECT--
> GET / HTTP/1.0
> Host: localhost
< HTTP/1.0 200 OK
<
< Hello world!
