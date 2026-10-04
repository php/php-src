--TEST--
dtls:// DTLS 1.3 handshake between a PHP server opted into 1.3 and a dtlsv1.3:// client
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
if (!in_array('dtlsv1.3', stream_get_transports(), true)) die('skip DTLS 1.3 not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls13_basic.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

$serverCode = <<<'CODE'
    // A server offers DTLS 1.3 only when asked: it cannot validate the peer address first
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'crypto_method' => STREAM_CRYPTO_METHOD_DTLS_ANY_SERVER,
    ]]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5);
    fwrite($peer, strtoupper(fread($peer, 8192)) . ' ' . stream_get_meta_data($peer)['crypto']['protocol']);
    fclose($peer);
    fclose($server);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['verify_peer' => false, 'verify_peer_name' => false]]);
    $client = stream_socket_client('dtlsv1.3://{{ ADDR }}', $errno, $errstr, 5,
        STREAM_CLIENT_CONNECT, $ctx);
    var_dump($client !== false);
    echo "client: ", stream_get_meta_data($client)['crypto']['protocol'], "\n";
    fwrite($client, "ping");
    var_dump(fread($client, 8192));
    fclose($client);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls13_basic.pem.tmp');
?>
--EXPECT--
bool(true)
client: DTLSv1.3
string(13) "PING DTLSv1.3"
