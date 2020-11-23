--TEST--
Bug #68920: peer_fingerprint input checks should be strict
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'bug68920.pem.tmp';

$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    stream_socket_accept($server, 30);
    stream_socket_accept($server, 30);
    stream_socket_accept($server, 30);
    stream_socket_accept($server, 30);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;

    phpt_wait();

    $ctx = stream_context_create(['ssl' => ['verify_peer'=> false, 'peer_fingerprint' => true]]);
    $sock = stream_socket_client($serverUri, $errno, $errstr, 30, $clientFlags, $ctx);
    var_dump($sock);

    $ctx = stream_context_create(['ssl' => ['verify_peer'=> false, 'peer_fingerprint' => null]]);
    $sock = stream_socket_client($serverUri, $errno, $errstr, 30, $clientFlags, $ctx);
    var_dump($sock);

    $ctx = stream_context_create(['ssl' => ['verify_peer'=> false, 'peer_fingerprint' => []]]);
    $sock = stream_socket_client($serverUri, $errno, $errstr, 30, $clientFlags, $ctx);
    var_dump($sock);

    $ctx = stream_context_create(['ssl' => ['verify_peer'=> false, 'peer_fingerprint' => ['foo']]]);
    $sock = stream_socket_client($serverUri, $errno, $errstr, 30, $clientFlags, $ctx);
    var_dump($sock);
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('bug68920', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'bug68920.pem.tmp');
?>
--EXPECTF--
Warning: stream_socket_client(): Expected peer fingerprint must be a string or an array in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): Unable to connect to %s (Unknown error) in %s on line %d
bool(false)

Warning: stream_socket_client(): Expected peer fingerprint must be a string or an array in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): Unable to connect to %s (Unknown error) in %s on line %d
bool(false)

Warning: stream_socket_client(): Invalid peer_fingerprint array; [algo => fingerprint] form required in %s on line %d

Warning: stream_socket_client(): peer_fingerprint match failure in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): Unable to connect to %s (Unknown error) in %s on line %d
bool(false)

Warning: stream_socket_client(): Invalid peer_fingerprint array; [algo => fingerprint] form required in %s on line %d

Warning: stream_socket_client(): peer_fingerprint match failure in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): Unable to connect to %s (Unknown error) in %s on line %d
bool(false)
