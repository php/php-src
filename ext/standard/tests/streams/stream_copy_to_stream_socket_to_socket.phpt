--TEST--
stream_copy_to_stream() socket to socket (splice both directions)
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php

$sourceCode = <<<'CODE'
    $server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
    phpt_notify_server_start($server);

    /* Send address again so the client can read it via phpt_wait(). */
    phpt_notify(message: stream_socket_get_name($server, false));

    $conn = stream_socket_accept($server);
    $data = str_repeat('test data ', 1000);
    fwrite($conn, $data);
    stream_socket_shutdown($conn, STREAM_SHUT_WR);

    /* Keep alive until client is done reading. */
    fread($conn, 1);

    fclose($conn);
    fclose($server);
CODE;

$destCode = <<<'CODE'
    $server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server);
    $result = stream_get_contents($conn);

    phpt_notify(message: strlen($result));
    phpt_notify(message: $result === str_repeat('test data ', 1000) ? "match" : "mismatch");

    fclose($conn);
    fclose($server);
CODE;

$clientCode = <<<'CODE'
    $sourceAddr = trim(phpt_wait("source"));
    $source = stream_socket_client("tcp://$sourceAddr", $errno, $errstr, 10);
    $dest = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 10);

    $copied = stream_copy_to_stream($source, $dest);
    var_dump($copied);

    stream_socket_shutdown($dest, STREAM_SHUT_WR);
    fclose($source);

    var_dump((int) trim(phpt_wait("dest")));
    var_dump(trim(phpt_wait("dest")) === "match");

    fclose($dest);
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, [
    'source' => $sourceCode,
    'dest' => $destCode,
]);
?>
--EXPECT--
int(10000)
int(10000)
bool(true)
