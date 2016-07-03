--TEST--
Bug #69215: Crypto servers should send client CA list
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
--FILE--
<?php
$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => __DIR__ . '/bug69215-server.pem',
        'passphrase' => 'elephpant',
        'cafile' => __DIR__ . '/bug69215-ca.pem',
        'verify_peer' => true,
        'verify_peer_name' => true,
        'peer_name' => 'bug69215-client',
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    stream_socket_accept($server, 30);
CODE;

$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'local_cert' => __DIR__ . '/bug69215-client.pem',
        'passphrase' => 'elephpant',
        'cafile' => __DIR__ . '/bug69215-ca.pem',
        'verify_peer' => true,
        'verify_peer_name' => true,
        'peer_name' => 'bug69215-server',
    ]]);

    phpt_wait();

    var_dump(stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
--EXPECTF--
resource(%d) of type (stream)

