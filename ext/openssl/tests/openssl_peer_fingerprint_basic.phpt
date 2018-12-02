--TEST--
Testing peer fingerprint on connection
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
        'local_cert' => __DIR__ . '/bug54992.pem'
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
CODE;

$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer'       => true,
        'cafile'            => __DIR__ . '/bug54992-ca.pem',
        'capture_peer_cert'    => true,
        'peer_name'          => 'bug54992.local',
    ]]);

    phpt_wait();

    // should be: 3610606deda596b3ae3859d33c4ce1d9
    stream_context_set_option($clientCtx, 'ssl', 'peer_fingerprint', '3610606deda596b3ae3859d33c4ce1da');
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx));

    stream_context_set_option($clientCtx, 'ssl', 'peer_fingerprint', [
        'sha256' => 'dffa72247ab7e44d94b2858528e3f67015925782148d2cf0b15cd82d1c931215',
    ]);
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx));
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
--EXPECTF--
Warning: stream_socket_client(): peer_fingerprint match failure in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to ssl://127.0.0.1:64321 (Unknown error) in %s on line %d
bool(false)
resource(%d) of type (stream)
