--TEST--
TLS session resumption - server with cache disabled
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_cache_disabled.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_cache' => false,  /* Disable session caching */
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    /* Accept two connections */
    for ($i = 0; $i < 2; $i++) {
        $client = @stream_socket_accept($server, 30);
        if ($client) {
            fwrite($client, "No cache connection " . ($i + 1) . "\n");
            fclose($client);
        }
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $sessionData = null;

    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => function($stream, $sessionId, $data) use (&$sessionData) {
            $sessionData = $data;
        }
    ]]);

    /* First connection */
    $client1 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);
    if ($client1) {
        echo trim(fgets($client1)) . "\n";
        $meta1 = stream_get_meta_data($client1);
        echo "First connection resumed: " . ($meta1['crypto']['session_reused'] ? "yes" : "no") . "\n";
        fclose($client1);
    }

    /* Second connection - server won't use cached session */
    $ctx2 = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_data' => $sessionData,
    ]]);

    $client2 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx2);
    if ($client2) {
        echo trim(fgets($client2)) . "\n";
        $meta2 = stream_get_meta_data($client2);
        echo "Second connection resumed: " . ($meta2['crypto']['session_reused'] ? "yes" : "no") . "\n";
        fclose($client2);
    }
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_disabled_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_cache_disabled.pem.tmp');
?>
--EXPECT--
No cache connection 1
First connection resumed: no
No cache connection 2
Second connection resumed: no
