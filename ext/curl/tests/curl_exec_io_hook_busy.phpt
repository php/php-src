--TEST--
curl_exec() under io_hooks: the handle refuses other use while the transfer waits
--EXTENSIONS--
curl
--FILE--
<?php

include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

[$ctl_server, $ctl_checker] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);
$ch = curl_init("http://$addr/");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_TIMEOUT, 5);

$scheduler->spawn(function () use ($server, $ctl_server) {
    $conn = stream_socket_accept($server, 5);
    $request = '';
    while (!str_ends_with($request, "\r\n\r\n")) {
        $chunk = fread($conn, 1024);
        if ($chunk === false || $chunk === '') break;
        $request .= $chunk;
    }
    fwrite($ctl_server, "r");
    fread($ctl_server, 1);
    fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Length: 5\r\n\r\nfirst");
    fclose($conn);
});

$scheduler->spawn(function () use ($ch) {
    var_dump(curl_exec($ch), curl_errno($ch));
});

$scheduler->spawn(function () use ($ch, $ctl_checker) {
    fread($ctl_checker, 1);
    $calls = [
        'curl_exec' => fn() => curl_exec($ch),
        'curl_reset' => fn() => curl_reset($ch),
        'curl_setopt' => fn() => curl_setopt($ch, CURLOPT_POSTFIELDS, ['a' => 'b']),
        'curl_setopt_array' => fn() => curl_setopt_array($ch, [CURLOPT_URL => 'http://example.com/']),
        'curl_copy_handle' => fn() => curl_copy_handle($ch),
        'curl_multi_add_handle' => fn() => curl_multi_add_handle(curl_multi_init(), $ch),
    ];
    foreach ($calls as $name => $call) {
        try {
            $call();
            echo "$name: no error\n";
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
    fwrite($ctl_checker, "g");
});

$scheduler->loop();

curl_setopt($ch, CURLOPT_URL, "http://$addr/");
echo "usable after the transfer\n";
?>
--EXPECT--
curl_exec(): Attempt to use cURL handle while curl_exec() is in progress on it
curl_reset(): Attempt to use cURL handle while curl_exec() is in progress on it
curl_setopt(): Attempt to use cURL handle while curl_exec() is in progress on it
curl_setopt_array(): Attempt to use cURL handle while curl_exec() is in progress on it
curl_copy_handle(): Attempt to use cURL handle while curl_exec() is in progress on it
curl_multi_add_handle(): Attempt to use cURL handle while curl_exec() is in progress on it
string(5) "first"
int(0)
usable after the transfer
