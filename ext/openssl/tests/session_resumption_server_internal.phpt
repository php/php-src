--TEST--
TLS session resumption - server internal cache
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_internal_cache.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_id_context' => 'test-server',
        'session_cache' => true,
        'session_cache_size' => 1024,
        'session_timeout' => 300,
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    /* Accept two connections */
    for ($i = 0; $i < 2; $i++) {
        $client = @stream_socket_accept($server, 30);
        if ($client) {
            fwrite($client, "Connection " . ($i + 1) . "\n");
            fclose($client);
        }
    }

    phpt_notify(message: "SERVER_DONE");
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
        $meta1 = stream_get_meta_data($client1);
        echo "Client first connection resumed: " . ($meta1['crypto']['session_reused'] ? "yes" : "no") . "\n";
        echo trim(fgets($client1)) . "\n";
        fclose($client1);
    }

    echo "Session data received: " . (strlen($sessionData) > 0 ? "YES" : "NO") . "\n";

    /* Second connection with session resumption */
    $ctx2 = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_data' => $sessionData,
    ]]);

    $client2 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx2);
    if ($client2) {
        $meta2 = stream_get_meta_data($client2);
        echo "Client second connection resumed: " . ($meta2['crypto']['session_reused'] ? "yes" : "no") . "\n";
        echo trim(fgets($client2)) . "\n";
        fclose($client2);
    }

    /* Wait for server */
    $result = phpt_wait();
    echo trim($result) . "\n";
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_internal_cache_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_internal_cache.pem.tmp');
?>
--EXPECTF--
Client first connection resumed: no
Connection 1
Session data received: YES
Client second connection resumed: yes
Connection 2
SERVER_DONE
