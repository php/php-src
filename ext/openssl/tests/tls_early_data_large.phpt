--TEST--
TLS 1.3 early data (0-RTT) larger than one read chunk
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (!defined('STREAM_CRYPTO_METHOD_TLSv1_3_SERVER')) die("skip TLS 1.3 not available");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_early_data_large.pem.tmp';

$serverCode = <<<'CODE'
    $early = '';
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_id_context' => 'early-data-large',
        'max_early_data' => 65536,
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
            fwrite($client, "len=" . strlen($early) . " ok=" .
                (int) ($early === str_repeat('A', 40000)) . " status=$status\n");
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
    $client1 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr,
        30, STREAM_CLIENT_CONNECT, $ctx);
    echo trim(fgets($client1)) . "\n";
    fclose($client1);

    $ctx2 = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_data' => $sessionData,
        'early_data' => str_repeat('A', 40000),
    ]]);
    $client2 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr,
        30, STREAM_CLIENT_CONNECT, $ctx2);
    echo trim(fgets($client2)) . "\n";
    fclose($client2);
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('tls_early_data_large', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_early_data_large.pem.tmp');
?>
--EXPECT--
len=0 ok=0 status=not_sent
len=40000 ok=1 status=accepted
