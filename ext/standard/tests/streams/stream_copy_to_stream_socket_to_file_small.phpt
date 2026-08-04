--TEST--
stream_copy_to_stream() 2048 bytes with socket as $source and file as $dest
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php

$serverCode = <<<'CODE'
    $server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server);
    fwrite($conn, str_repeat("a", 2048));
    stream_socket_shutdown($conn, STREAM_SHUT_WR);

    /* Keep alive until client is done reading. */
    fread($conn, 1);

    fclose($conn);
    fclose($server);
CODE;

$clientCode = <<<'CODE'
    $source = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 10);
    $tmp = tmpfile();

    stream_copy_to_stream($source, $tmp);

    fseek($tmp, 0, SEEK_SET);
    var_dump(stream_get_contents($tmp));

    fclose($tmp);
    fclose($source);
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECTF--
string(2048) "aaaaa%saaa"
