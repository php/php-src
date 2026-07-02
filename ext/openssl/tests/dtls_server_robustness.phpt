--TEST--
dtls:// server: bogus datagrams are ignored, a real peer still connects
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_robustness.pem.tmp';
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
        fwrite($peer, strtoupper(fread($peer, 8192)));
        fclose($peer);
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    // Flood the server's port with garbage that is not a valid (cookied)
    // ClientHello; the DTLSv1_listen loop must ignore it.
    $junk = stream_socket_client('udp://{{ ADDR }}', $errno, $errstr, 2, STREAM_CLIENT_CONNECT);
    for ($i = 0; $i < 8; $i++) {
        fwrite($junk, random_bytes(80));
    }
    fclose($junk);

    // A real DTLS client must still complete the handshake.
    $ctx = stream_context_create(['ssl' => ['verify_peer' => false]]);
    $client = stream_socket_client('dtls://{{ ADDR }}', $errno, $errstr, 5,
        STREAM_CLIENT_CONNECT, $ctx);
    var_dump($client !== false);
    fwrite($client, "ping");
    var_dump(fread($client, 8192));
    fclose($client);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_robustness.pem.tmp');
?>
--EXPECT--
bool(true)
string(4) "PING"
