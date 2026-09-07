--TEST--
stream_copy_to_stream() socket to file with a maxlength shorter than the data
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
    fwrite($conn, str_repeat("a", 10000));
    stream_socket_shutdown($conn, STREAM_SHUT_WR);

    /* Keep alive until client is done reading. */
    fread($conn, 1);

    fclose($conn);
    fclose($server);
CODE;

$clientCode = <<<'CODE'
    $source = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 10);
    $tmp = tmpfile();

    /* Only 4096 of the 10000 available bytes must be copied. */
    $copied = stream_copy_to_stream($source, $tmp, 4096);
    var_dump($copied);

    fseek($tmp, 0, SEEK_SET);
    $content = stream_get_contents($tmp);
    var_dump(strlen($content));
    var_dump($content === str_repeat("a", 4096));

    fclose($tmp);
    fclose($source);
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
int(4096)
int(4096)
bool(true)
