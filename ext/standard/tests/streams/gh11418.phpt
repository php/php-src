--TEST--
GH-11418: fgets on a redis socket connection fails on PHP 8.3
--FILE--
<?php

$serverCode = <<<'CODE'
$server = stream_socket_server('tcp://127.0.0.1:0');
phpt_notify_server_start($server);

$conn = stream_socket_accept($server);

fwrite($conn, "Hi Hello"); // 8 bytes
usleep(50000);
fwrite($conn, " World\n"); // 8 bytes

fclose($conn);
fclose($server);
CODE;

$clientCode = <<<'CODE'

$fp = fsockopen("tcp://{{ ADDR }}");

echo fread($fp, 3);
echo fgets($fp);

CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

?>
--EXPECT--
Hi Hello World
