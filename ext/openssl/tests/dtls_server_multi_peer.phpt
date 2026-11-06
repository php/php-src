--TEST--
dtls:// server: several peers share one socket and are demultiplexed by address
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_multi_peer.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peers = [];
    for ($i = 0; $i < 3; $i++) {
        $peer = stream_socket_accept($server, 5, $addr);
        if (stream_socket_get_name($peer, true) !== $addr) {
            echo "peer name mismatch\n";
        }
        $peers[] = $peer;
    }
    // Read in reverse order: datagrams for the other peers arrive meanwhile and are queued
    foreach (array_reverse($peers, true) as $i => $peer) {
        $data = fread($peer, 8192);
        fwrite($peer, strtoupper($data) . " from peer $i");
    }
    foreach ($peers as $peer) {
        fclose($peer);
    }
    fclose($server);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['verify_peer' => false, 'verify_peer_name' => false]]);
    $clients = [];
    for ($i = 0; $i < 3; $i++) {
        $clients[$i] = stream_socket_client('dtls://{{ ADDR }}', $errno, $errstr, 5,
            STREAM_CLIENT_CONNECT, $ctx);
        var_dump($clients[$i] !== false);
    }
    foreach ($clients as $i => $client) {
        fwrite($client, "ping $i");
    }
    foreach ($clients as $i => $client) {
        var_dump(fread($client, 8192));
    }
    foreach ($clients as $client) {
        fclose($client);
    }
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_multi_peer.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(18) "PING 0 from peer 0"
string(18) "PING 1 from peer 1"
string(18) "PING 2 from peer 2"
