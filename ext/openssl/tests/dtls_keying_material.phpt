--TEST--
dtls://: RFC 5705 exported keying material matches on both peers
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_keying_material.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'keying_material_label' => 'EXTRACTOR-dtls_srtp',
        'keying_material_length' => 60,
    ]]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5);
    $meta = stream_get_meta_data($peer);
    fwrite($peer, bin2hex($meta['crypto']['keying_material']));
    fclose($peer);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'keying_material_label' => 'EXTRACTOR-dtls_srtp',
        'keying_material_length' => 60,
    ]]);
    $client = stream_socket_client('dtls://{{ ADDR }}', $errno, $errstr, 5,
        STREAM_CLIENT_CONNECT, $ctx);
    $meta = stream_get_meta_data($client);
    $km = $meta['crypto']['keying_material'];
    var_dump(strlen($km) === 60);
    var_dump(fread($client, 8192) === bin2hex($km));
    fclose($client);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_keying_material.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
