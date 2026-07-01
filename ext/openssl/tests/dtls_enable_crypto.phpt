--TEST--
udp:// + stream_socket_enable_crypto(): DTLS over an existing udp:// socket
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_enable_crypto.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5);
    if ($peer === false) {
        echo "accept failed\n";
    } else {
        $data = fread($peer, 8192);
        fwrite($peer, strtoupper($data));
        fclose($peer);
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['verify_peer' => false]]);
    $client = stream_socket_client('udp://{{ ADDR }}', $errno, $errstr, 5,
        STREAM_CLIENT_CONNECT, $ctx);
    var_dump($client !== false);
    var_dump(stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_DTLS_CLIENT));
    fwrite($client, "ping");
    var_dump(fread($client, 8192));
    fclose($client);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_enable_crypto.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
string(4) "PING"
