--TEST--
Bug #48182: ssl handshake fails during asynchronous socket connection
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
if (OPENSSL_VERSION_NUMBER < 0x009070af) die("skip openssl version too low");
--FILE--
<?php
$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => __DIR__ . '/bug54992.pem'
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    $client = @stream_socket_accept($server, 1);

    $data = "Sending bug48182\n" . fread($client, 8192);
    fwrite($client, $data);
CODE;

$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT | STREAM_CLIENT_ASYNC_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'cafile' => __DIR__ . '/bug54992-ca.pem',
        'peer_name' => 'bug54992.local'
    ]]);

    phpt_wait();
    $client = stream_socket_client($serverUri, $errno, $errstr, 10, $clientFlags, $clientCtx);

    $data = "Sending data over to SSL server in async mode with contents like Hello World\n";

    fwrite($client, $data);
    echo fread($client, 1024);
CODE;

echo "Running bug48182\n";

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
--EXPECTF--
Running bug48182
Sending bug48182
Sending data over to SSL server in async mode with contents like Hello World
