--TEST--
curl_exec() under IO hooks: a large upload to a slow reader completes
--EXTENSIONS--
curl
--FILE--
<?php

include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$size = 4 * 1024 * 1024;
$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);

$scheduler->spawn(function () use ($server, $size) {
    $conn = stream_socket_accept($server, 10);
    $headers = '';
    while (!str_contains($headers, "\r\n\r\n")) {
        $headers .= fread($conn, 8192);
    }
    [$headers, $body] = explode("\r\n\r\n", $headers, 2);
    preg_match('/Content-Length: (\d+)/i', $headers, $m);
    $length = (int) $m[1];
    if (str_contains($headers, 'Expect: 100-continue')) {
        fwrite($conn, "HTTP/1.1 100 Continue\r\n\r\n");
    }
    $received = strlen($body);
    // A reader slower than the sender, so the socket's send buffer fills up
    while ($received < $length) {
        $chunk = fread($conn, 65536);
        if ($chunk === false || $chunk === '') {
            break;
        }
        $received += strlen($chunk);
        usleep(2000);
    }
    $reply = "received $received";
    fwrite($conn, "HTTP/1.1 200 OK\r\nContent-Length: " . strlen($reply) . "\r\nConnection: close\r\n\r\n$reply");
    fclose($conn);
});

$scheduler->spawn(function () use ($addr, $size) {
    $ch = curl_init("http://$addr/upload");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, str_repeat('u', $size));
    curl_setopt($ch, CURLOPT_TIMEOUT, 60);
    $body = curl_exec($ch);
    if ($body === false) {
        echo "curl error ", curl_errno($ch), ": ", curl_error($ch), "\n";
    } else {
        echo $body, "\n";
    }
});

$scheduler->loop();
?>
--EXPECT--
received 4194304
