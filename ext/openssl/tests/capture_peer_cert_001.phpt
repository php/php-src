--TEST--
capture_peer_cert context captures on verify failure
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'capture_peer_cert_001.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'capture_peer_cert_001-ca.pem.tmp';

$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s'
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    @stream_socket_accept($server, 1);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$peerName = 'capture_peer_cert_001';
$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'capture_peer_cert' => true,
        'cafile' => '%s'
    ]]);

    phpt_wait();
    $client = @stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx);
    $cert = stream_context_get_options($clientCtx)['ssl']['peer_certificate'];
    var_dump(openssl_x509_parse($cert)['subject']['CN']);
CODE;
$clientCode = sprintf($clientCode, $cacertFile);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAsFileWithKey($peerName, $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'capture_peer_cert_001.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'capture_peer_cert_001-ca.pem.tmp');
?>
--EXPECTF--
string(%d) "capture_peer_cert_001"
