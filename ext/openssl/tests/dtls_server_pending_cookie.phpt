--TEST--
dtls:// server: peers that never return the cookie do not hold the pending slots
--EXTENSIONS--
openssl
--SKIPIF--
<?php
require __DIR__ . '/dtls_skipif.inc';
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_pending_cookie.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('dtls-server', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'dtls_max_pending' => 4,
        'dtls_pending_timeout' => 30,
    ]]);
    $server = stream_socket_server('dtls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 10);
    if ($peer !== false) {
        fwrite($peer, strtoupper(fread($peer, 8192)));
        fclose($peer);
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = ['ssl' => ['verify_peer' => false, 'verify_peer_name' => false]];

    // A ClientHello to replay: a client of our own could answer the cookie request when the
    // server is quick enough.
    $sink = stream_socket_server('udp://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND);
    $probe = stream_socket_client('udp://' . stream_socket_get_name($sink, false), $errno, $errstr, 2,
        STREAM_CLIENT_CONNECT, stream_context_create($ctx));
    stream_set_blocking($probe, false);
    @stream_socket_enable_crypto($probe, true, STREAM_CRYPTO_METHOD_DTLS_ANY_CLIENT);
    $hello = stream_socket_recvfrom($sink, 65535);

    // As many peers as there are pending slots send a ClientHello and never return the cookie.
    $silent = [];
    for ($i = 0; $i < 4; $i++) {
        $s = stream_socket_client('udp://{{ ADDR }}', $errno, $errstr, 2);
        fwrite($s, $hello);
        $silent[] = $s;
    }
    usleep(200000);

    // A real peer still connects, well before dtls_pending_timeout reaps them.
    $client = stream_socket_client('dtls://{{ ADDR }}', $errno, $errstr, 3,
        STREAM_CLIENT_CONNECT, stream_context_create($ctx));
    var_dump($client !== false);
    if ($client !== false) {
        fwrite($client, "ping");
        var_dump(fread($client, 8192));
        fclose($client);
    }
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'dtls_server_pending_cookie.pem.tmp');
?>
--EXPECT--
bool(true)
string(4) "PING"
