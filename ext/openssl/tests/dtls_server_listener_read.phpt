--TEST--
dtls:// server: a read on the listener does not take the datagrams of its peers
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_listener_read.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5);
    phpt_notify();
    phpt_wait();

    // The record of the peer is in the socket of the listener.
    stream_set_timeout($server, 1);
    $read = fread($server, 2048);
    $timedOut = stream_get_meta_data($server)['timed_out'];
    $from = @stream_socket_recvfrom($server, 2048);
    stream_set_blocking($server, false);
    $nonBlocking = fread($server, 2048);

    stream_set_timeout($peer, 5);
    fwrite($peer, json_encode([$read, $timedOut, $from, $nonBlocking, feof($server), fread($peer, 64)]));
    fread($peer, 1);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['verify_peer' => false, 'verify_peer_name' => false]]);
    $client = stream_socket_client('dtls://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
    phpt_wait();
    fwrite($client, "ping");
    phpt_notify();
    stream_set_timeout($client, 10);
    echo fread($client, 1024), "\n";
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_listener_read.pem.tmp');
?>
--EXPECT--
[false,true,false,false,false,"ping"]
