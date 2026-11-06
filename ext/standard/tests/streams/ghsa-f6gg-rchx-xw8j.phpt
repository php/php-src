--TEST--
GHSA-f6gg-rchx-xw8j: mixed-case FTPS URLs must enable TLS
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$serverCode = <<<'CODE'
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server);
    fwrite($conn, "220 Ready\r\n");
    $command = stream_get_line($conn, 8192, "\r\n");

    fclose($conn);
    fclose($server);
    phpt_notify(message: $command);
CODE;

$clientCode = <<<'CODE'
    var_dump(@fopen('FTPS://{{ ADDR }}/secret', 'r'));
    echo phpt_wait();
CODE;

include sprintf('%s/../../../openssl/tests/ServerClientTestCase.inc', __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
bool(false)
AUTH TLS
