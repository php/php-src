--TEST--
curl_exec() under io_hooks: a CURLOPT_FILE stream closed while the transfer waits ends the transfer
--EXTENSIONS--
curl
--FILE--
<?php

include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);
$file = tmpfile();

/* The request has arrived, so curl_exec() waits for the response: the close
 * lands inside that wait */
$scheduler->spawn(function () use ($server, $file) {
    $conn = stream_socket_accept($server, 5);
    $request = '';
    while (!str_ends_with($request, "\r\n\r\n")) {
        $chunk = fread($conn, 1024);
        if ($chunk === false || $chunk === '') break;
        $request .= $chunk;
    }
    fclose($file);
    fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Length: 5\r\n\r\nhello");
    fclose($conn);
});

$scheduler->spawn(function () use ($addr, $file) {
    $ch = curl_init("http://$addr/");
    curl_setopt($ch, CURLOPT_FILE, $file);
    curl_setopt($ch, CURLOPT_TIMEOUT, 5);
    var_dump(curl_exec($ch), curl_errno($ch) === CURLE_WRITE_ERROR);
});

$scheduler->loop();
?>
--EXPECTF--
Warning: curl_exec(): CURLOPT_FILE resource has gone away, resetting to default in %s on line %d
bool(false)
bool(true)
