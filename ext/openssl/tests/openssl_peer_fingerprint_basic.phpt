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

    // Run the following to get actual md5 (from sources root):
    // openssl x509 -noout -fingerprint -md5 -inform pem -in ext/openssl/tests/bug54992.pem | cut -d '=' -f 2 | tr -d ':' | tr 'A-F' 'a-f'
    // Currently it's 4edbbaf40a6a4b6af22b6d6d9818378f
    // One below is intentionally broken (compare the last character):
    stream_context_set_option($clientCtx, 'ssl', 'peer_fingerprint', '4edbbaf40a6a4b6af22b6d6d98183780');
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx));

    // Run the following to get actual sha256 (from sources root):
    // openssl x509 -noout -fingerprint -sha256 -inform pem -in ext/openssl/tests/bug54992.pem | cut -d '=' -f 2 | tr -d ':' | tr 'A-F' 'a-f'
    stream_context_set_option($clientCtx, 'ssl', 'peer_fingerprint', [
        'sha256' => 'b1d480a2f83594fa243d26378cf611f334d369e59558d87e3de1abe8f36cb997',
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
