--TEST--
dtls:// server: verify the client certificate (mutual authentication)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$caFile     = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_verify-ca.pem.tmp';
$serverCert = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_verify-server.pem.tmp';
$clientCert = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_verify-client.pem.tmp';

include 'CertificateGenerator.inc';
$gen = new CertificateGenerator();
$gen->saveCaCert($caFile);
$gen->saveNewCertAsFileWithKey('dtls-server', $serverCert);
$gen->saveNewCertAsFileWithKey('dtls-client', $clientCert);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'verify_peer' => true,
        'verify_peer_name' => false,
        'cafile' => '%s',
    ]]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = @stream_socket_accept($server, 5);
    if ($peer !== false) {
        fwrite($peer, "verified");
        fclose($peer);
    }
CODE;
$serverCode = sprintf($serverCode, $serverCert, $caFile);

include 'ServerClientTestCase.inc';

// 1) The client presents a CA-signed certificate -> the server accepts it.
$clientOk = sprintf(<<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s', 'verify_peer' => false]]);
    $client = stream_socket_client('dtls://{{ ADDR }}', $errno, $errstr, 5,
        STREAM_CLIENT_CONNECT, $ctx);
    var_dump($client !== false);
    var_dump(fread($client, 8192));
CODE, $clientCert);
ServerClientTestCase::getInstance()->run($clientOk, $serverCode);

// 2) The client presents no certificate -> the handshake is rejected.
$clientNoCert = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['verify_peer' => false]]);
    $client = @stream_socket_client('dtls://{{ ADDR }}', $errno, $errstr, 5,
        STREAM_CLIENT_CONNECT, $ctx);
    var_dump($client === false);
CODE;
ServerClientTestCase::getInstance()->run($clientNoCert, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_verify-ca.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_verify-server.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_verify-client.pem.tmp');
?>
--EXPECT--
bool(true)
string(8) "verified"
bool(true)
