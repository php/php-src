--TEST--
Allow host name mismatch when "verify_host" disabled
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_verify_peer_name_002.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_verify_peer_name_002-ca.pem.tmp';

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

$actualPeerName = 'stream_verify_peer_name_002';
$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => true,
        'cafile' => '%s',
        'verify_peer_name' => false
    ]]);

    phpt_wait();
    $client = stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx);

    var_dump($client);
CODE;
$clientCode = sprintf($clientCode, $cacertFile);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAsFileWithKey($actualPeerName, $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_verify_peer_name_002.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_verify_peer_name_002-ca.pem.tmp');
?>
--EXPECTF--
resource(%d) of type (stream)
