--TEST--
curl_exec() under io_hooks: a CURLOPT_INFILE stream closed while the upload waits ends the transfer
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
fwrite($file, 'hello');
rewind($file);

/* With "Expect: 100-continue" curl reads no body before the 100, so it waits
 * for it: the close lands inside that wait, before the first read */
$scheduler->spawn(function () use ($server, $file) {
    $conn = stream_socket_accept($server, 5);
    $request = '';
    while (!str_contains($request, "\r\n\r\n")) {
        $chunk = fread($conn, 1024);
        if ($chunk === false || $chunk === '') break;
        $request .= $chunk;
    }
    fclose($file);
    fwrite($conn, "HTTP/1.1 100 Continue\r\n\r\n");
    while (!feof($conn)) {
        if (fread($conn, 1024) === false) break;
    }
    fclose($conn);
});

$scheduler->spawn(function () use ($addr, $file) {
    $ch = curl_init("http://$addr/");
    curl_setopt($ch, CURLOPT_UPLOAD, true);
    curl_setopt($ch, CURLOPT_INFILE, $file);
    curl_setopt($ch, CURLOPT_INFILESIZE, 5);
    curl_setopt($ch, CURLOPT_HTTPHEADER, ['Expect: 100-continue']);
    curl_setopt($ch, CURLOPT_TIMEOUT, 5);
    var_dump(curl_exec($ch), curl_errno($ch) === CURLE_ABORTED_BY_CALLBACK);
});

$scheduler->loop();
?>
--EXPECTF--
Warning: curl_exec(): CURLOPT_INFILE resource has gone away, resetting to default in %s on line %d
bool(false)
bool(true)
