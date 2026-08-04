--TEST--
stream_copy_to_stream() with socket as $source and STDOUT as $dest
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php

$serverCode = <<<'CODE'
    $server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server, 5);
    fwrite($conn, "data to stdout\n");
    fclose($conn);
    fclose($server);
CODE;

$clientCode = <<<'CODE'
    $fd = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 10);

    stream_copy_to_stream($fd, STDOUT);

    fclose($fd);
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
data to stdout
