--TEST--
Peer verification enabled for client streams
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'peer_verification.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'peer_verification-ca.pem.tmp';

$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s'
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    for ($i = 0; $i < 5; $i++) {
        @stream_socket_accept($server, 1);
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$peerName = 'peer_verification';
$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $caFile = '%s';

    phpt_wait();

    // Expected to fail -- untrusted server cert and no CA File present
    var_dump(@stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags));

    // Expected to fail -- untrusted server cert and no CA File present
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => true,
    ]]);
    var_dump(@stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));

    // Should succeed with peer verification disabled in context
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
    ]]);
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));

    // Should succeed with CA file specified in context
    $clientCtx = stream_context_create(['ssl' => [
        'cafile'   => $caFile,
        'peer_name' => '%s',
    ]]);
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));
CODE;
$clientCode = sprintf($clientCode, $cacertFile, $peerName);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAsFileWithKey($peerName, $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'peer_verification.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'peer_verification-ca.pem.tmp');
?>
--EXPECTF--
bool(false)
bool(false)
resource(%d) of type (stream)
resource(%d) of type (stream)
