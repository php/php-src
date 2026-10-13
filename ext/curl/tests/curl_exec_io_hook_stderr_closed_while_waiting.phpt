--TEST--
curl_exec() under io_hooks: a CURLOPT_STDERR stream closed while the transfer waits is not written to
--EXTENSIONS--
curl
--FILE--
<?php

include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);
$log = tmpfile();

/* The request has arrived, so curl_exec() waits for the response: the close
 * lands inside that wait */
$scheduler->spawn(function () use ($server, $log) {
    $conn = stream_socket_accept($server, 5);
    $request = '';
    while (!str_ends_with($request, "\r\n\r\n")) {
        $chunk = fread($conn, 1024);
        if ($chunk === false || $chunk === '') break;
        $request .= $chunk;
    }
    fclose($log);
    fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Length: 5\r\n\r\nhello");
    fclose($conn);
});

$scheduler->spawn(function () use ($addr, $log) {
    $ch = curl_init("http://$addr/");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_VERBOSE, true);
    curl_setopt($ch, CURLOPT_STDERR, $log);
    curl_setopt($ch, CURLOPT_TIMEOUT, 5);
    var_dump(curl_exec($ch));
});

$scheduler->loop();
?>
--EXPECTF--
%A< HTTP/1.0 200 OK%A
string(5) "hello"
