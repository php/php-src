--TEST--
GH-23425 (sapi_cli_server_send_headers() does not check the return value of php_cli_server_client_send_through())
--EXTENSIONS--
sockets
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";

$info = php_cli_server_start(<<<'PHP'
ignore_user_abort(true);
usleep(300000);
header('X-Test: 1');
echo 'x';
file_put_contents(__DIR__ . '/result.txt', headers_sent() ? 'sent' : 'not-sent');
PHP);

// Connect the usual way, then drop to the socket extension only to force a
// hard reset (SO_LINGER=0) instead of a graceful close, so the server's
// header write fails deterministically while the script is still running
// (ignore_user_abort(true)).
$stream = stream_socket_client("tcp://" . PHP_CLI_SERVER_ADDRESS);
$sock = socket_import_stream($stream);
socket_write($sock, "GET /index.php HTTP/1.1\r\nHost: " . PHP_CLI_SERVER_HOSTNAME . "\r\nConnection: close\r\n\r\n");
socket_set_option($sock, SOL_SOCKET, SO_LINGER, ['l_onoff' => 1, 'l_linger' => 0]);
socket_close($sock);

$result_file = $info->docRoot . '/result.txt';
for ($i = 0; $i < 40 && !file_exists($result_file); $i++) {
    usleep(50000);
}

echo file_get_contents($result_file), "\n";
?>
--EXPECT--
not-sent
