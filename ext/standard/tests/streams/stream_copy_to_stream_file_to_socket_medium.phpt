--TEST--
stream_copy_to_stream() 16k with file as $source and socket as $dest
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
    $data = str_repeat('data', 4096);
    $result = stream_get_contents($conn);

    phpt_notify(message: strlen($result));
    phpt_notify(message: $result === $data ? "match" : "mismatch");

    fclose($conn);
    fclose($server);
CODE;

$clientCode = <<<'CODE'
    $src = tmpfile();
    $data = str_repeat('data', 4096);
    fwrite($src, $data);
    rewind($src);

    $dest = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 10);
    $copied = stream_copy_to_stream($src, $dest);
    var_dump($copied);

    fclose($dest);
    fclose($src);

    var_dump((int) trim(phpt_wait()));
    var_dump(trim(phpt_wait()) === "match");
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
int(16384)
int(16384)
bool(true)
