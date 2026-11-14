--TEST--
dtls:// server: peers that are never accepted cannot exhaust memory
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_unaccepted_memory.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

$serverCode = <<<'CODE'
    ini_set('memory_limit', '8M');
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s', 'dtls_max_pending' => 4]]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5);
    stream_set_timeout($peer, 20);
    fwrite($peer, strtoupper(fread($peer, 8192)));
    fclose($peer);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['verify_peer' => false, 'verify_peer_name' => false]]);
    $client = stream_socket_client('dtls://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);

    // Peers that complete the handshake, are never accepted and send as much as they can.
    // Only dtls_max_pending of them get a handshake.
    $big = str_repeat('A', 8192);
    $flood = [];
    $handshakes = 0;
    for ($i = 0; $i < 8; $i++) {
        $c = stream_socket_client('udp://{{ ADDR }}', $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $ctx);
        if (!@stream_socket_enable_crypto($c, true, STREAM_CRYPTO_METHOD_DTLS_ANY_CLIENT)) {
            continue;
        }
        $handshakes++;
        for ($k = 0; $k < 256; $k++) {
            stream_socket_sendto($c, $big);
            if ($k % 8 == 0) {
                usleep(5000);
            }
        }
        $flood[] = $c;
    }

    var_dump($handshakes);

    // The server is still serving the accepted peer.
    stream_set_timeout($client, 5);
    fwrite($client, "ping");
    var_dump(fread($client, 8192));
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_unaccepted_memory.pem.tmp');
?>
--EXPECT--
int(4)
string(4) "PING"
