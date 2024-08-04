--TEST--
GH-13860 (Incorrect PHP_STREAM_OPTION_CHECK_LIVENESS case in ext/openssl/xp_ssl.c - causing use of dead socket)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$serverCode = <<<'CODE'
    $serverUri = "tcp://127.0.0.1:64326";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create();

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    $client = @stream_socket_accept($server);
    if ($client) {
        phpt_wait();
        fwrite($client, "xx");
        fclose($client);
        phpt_notify();
    }
CODE;

$clientCode = <<<'CODE'
    $serverUri = "tcp://127.0.0.1:64326";
    $clientFlags = STREAM_CLIENT_CONNECT;

    phpt_wait();
    $fp = stream_socket_client($serverUri);
    stream_set_blocking($fp, false);

    fread($fp, 2);

    phpt_notify();
    while (!($in = fread($fp, 2))) {
        usleep(1000);
    }
    phpt_wait();
    var_dump(feof($fp));
    fclose($fp);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
bool(true)
