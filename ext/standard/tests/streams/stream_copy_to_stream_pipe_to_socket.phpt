--TEST--
stream_copy_to_stream() with a pipe as $source and a socket as $dest (splice from pipe)
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
    phpt_notify(message: $result === str_repeat("p", 5000) ? "match" : "mismatch");

    fclose($conn);
    fclose($server);
CODE;

$clientCode = <<<'CODE'
    /* A real pipe as the source exercises the splice-from-pipe path; with a
     * socket destination it must use the corked (SPLICE_F_MORE) variant. */
    $descriptors = [1 => ['pipe', 'w']];
    $proc = proc_open(
        [PHP_BINARY, '-n', '-r', 'echo str_repeat("p", 5000);'],
        $descriptors,
        $pipes
    );

    $source = $pipes[1];
    $dest = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 10);

    $copied = stream_copy_to_stream($source, $dest);
    var_dump($copied);

    fclose($dest);
    fclose($source);
    proc_close($proc);

    var_dump((int) trim(phpt_wait()));
    var_dump(trim(phpt_wait()) === "match");
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
int(5000)
int(5000)
bool(true)
