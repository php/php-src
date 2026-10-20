--TEST--
TLS 1.3 early data option is ignored when the session does not allow 0-RTT
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (!defined('STREAM_CRYPTO_METHOD_TLSv1_3_SERVER')) die("skip TLS 1.3 not available");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_early_data_unsupported.pem.tmp';

/* Server does NOT enable early data, so a resumed session must not carry a
 * 0-RTT limit and the client must fall back to a normal handshake. */
$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_cache' => true,
        'session_id_context' => 'early-data-off',
    ]]);
    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);
    for ($i = 0; $i < 2; $i++) {
        $client = @stream_socket_accept($server, 30);
        if ($client) {
            fwrite($client, "hello\n");
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
        'early_data' => 'ping-0rtt',
    ]]);
    $client2 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr,
        30, STREAM_CLIENT_CONNECT, $ctx2);
    echo trim(fgets($client2)) . "\n";
    $meta2 = stream_get_meta_data($client2);
    echo "reused: " . ($meta2['crypto']['session_reused'] ? 'yes' : 'no') . "\n";
    echo "early_data key present: " . (array_key_exists('early_data', $meta2['crypto']) ? 'yes' : 'no') . "\n";
    fclose($client2);
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('tls_early_data_unsupported', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_early_data_unsupported.pem.tmp');
?>
--EXPECT--
hello
hello
reused: yes
early_data key present: no
