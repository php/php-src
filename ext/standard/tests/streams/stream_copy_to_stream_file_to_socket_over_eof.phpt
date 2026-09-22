--TEST--
stream_copy_to_stream() file to socket with a maxlength larger than the file (sendfile stops at EOF)
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
    phpt_notify(message: $result === str_repeat("A", 4096) ? "match" : "mismatch");

    fclose($conn);
    fclose($server);
CODE;

$clientCode = <<<'CODE'
    $src = tmpfile();
    fwrite($src, str_repeat("A", 4096));
    rewind($src);

    $dest = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 10);

    /* maxlen exceeds the file size: sendfile must stop at EOF and report only
     * the bytes actually available rather than blocking for the full maxlen. */
    $copied = stream_copy_to_stream($src, $dest, 100000);
    var_dump($copied);

    /* Nothing left to read once the whole file has been consumed. */
    var_dump(strlen(fread($src, 4096)));

    fclose($dest);
    fclose($src);

    var_dump((int) trim(phpt_wait()));
    var_dump(trim(phpt_wait()) === "match");
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
int(4096)
int(0)
int(4096)
bool(true)
