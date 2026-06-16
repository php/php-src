--TEST--
stream_copy_to_stream() file to socket with a maxlength shorter than the file (bounded sendfile + source offset)
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
    $result = stream_get_contents($conn);

    phpt_notify(message: strlen($result));
    phpt_notify(message: $result === str_repeat("A", 8192) ? "match" : "mismatch");

    fclose($conn);
    fclose($server);
CODE;

$clientCode = <<<'CODE'
    $src = tmpfile();
    fwrite($src, str_repeat("A", 8192) . str_repeat("B", 8192));
    rewind($src);

    $dest = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 10);

    /* Only the first 8192 bytes must be sent: the bounded sendfile path has to
     * stop at maxlen rather than streaming to EOF. */
    $copied = stream_copy_to_stream($src, $dest, 8192);
    var_dump($copied);

    /* The source position must have advanced by exactly maxlen, so the kernel
     * offload restored the descriptor offset to the maxlen boundary. */
    $rest = fread($src, 8192);
    var_dump(strlen($rest));
    var_dump($rest === str_repeat("B", 8192));

    fclose($dest);
    fclose($src);

    var_dump((int) trim(phpt_wait()));
    var_dump(trim(phpt_wait()) === "match");
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
int(8192)
int(8192)
bool(true)
int(8192)
bool(true)
