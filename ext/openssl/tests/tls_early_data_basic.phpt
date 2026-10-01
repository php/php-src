--TEST--
TLS 1.3 early data (0-RTT) basic client/server round-trip
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (!defined('STREAM_CRYPTO_METHOD_TLSv1_3_SERVER')) die("skip TLS 1.3 not available");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_early_data_basic.pem.tmp';

$serverCode = <<<'CODE'
    $early = '';
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_id_context' => 'early-data-test',
        'max_early_data' => 16384,
        'early_data_cb' => function ($stream, string $data) use (&$early) {
            $early .= $data;
        },
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    for ($i = 0; $i < 2; $i++) {
        $early = '';
        $client = @stream_socket_accept($server, 30);
        if ($client) {
            $meta = stream_get_meta_data($client);
            $status = $meta['crypto']['early_data'] ?? 'none';
            fwrite($client, "early=[$early] status=$status\n");
            fclose($client);
        }
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $sessionData = null;

    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => function ($stream, $session) use (&$sessionData) {
            $sessionData = $session;
        },
    ]]);

    /* First connection: full handshake, capture the resumable session */
    $client1 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr,
        30, STREAM_CLIENT_CONNECT, $ctx);
    echo trim(fgets($client1)) . "\n";
    fclose($client1);

    /* Second connection: resume and send 0-RTT early data */
    $ctx2 = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_data' => $sessionData,
        'early_data' => 'ping-0rtt',
    ]]);

    $client2 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr,
        30, STREAM_CLIENT_CONNECT, $ctx2);
    echo trim(fgets($client2)) . "\n";
    $meta2 = stream_get_meta_data($client2);
    echo "client early_data: " . ($meta2['crypto']['early_data'] ?? 'none') . "\n";
    fclose($client2);
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('tls_early_data_basic', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_early_data_basic.pem.tmp');
?>
--EXPECT--
early=[] status=not_sent
early=[ping-0rtt] status=accepted
client early_data: accepted
