--TEST--
dtls:// client: a non-blocking handshake reports WANT_READ until it completes
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_client_nonblocking.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5);
    fwrite($peer, strtoupper(fread($peer, 8192)));
    fclose($peer);
    fclose($server);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['verify_peer' => false, 'verify_peer_name' => false]]);
    $client = stream_socket_client('udp://{{ ADDR }}', $errno, $errstr, 5,
        STREAM_CLIENT_CONNECT, $ctx);
    stream_set_blocking($client, false);

    $rounds = 0;
    $waited = 0;
    while (($r = stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_DTLS_ANY_CLIENT)) === 0) {
        $rounds++;
        $read = [$client]; $write = $except = [];
        if (stream_select($read, $write, $except, 5) > 0) {
            $waited++;
        }
        if ($rounds > 100) break;
    }
    var_dump($r);
    var_dump($rounds > 0 && $waited > 0);

    stream_set_blocking($client, true);
    fwrite($client, "ping");
    var_dump(fread($client, 8192));
    fclose($client);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_client_nonblocking.pem.tmp');
?>
--EXPECT--
bool(true)
bool(true)
string(4) "PING"
