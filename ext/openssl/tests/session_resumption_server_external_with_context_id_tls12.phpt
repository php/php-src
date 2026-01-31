--TEST--
TLS session resumption - server external cache callbacks with context id for TLS 1.2
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_external_proper.pem.tmp';

$serverCode = <<<'CODE'
    $sessionStore = [];
    $newCbCalled = false;
    $getCbCalled = false;

    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_id_context' => 'test-server',  // Proper configuration
        'session_new_cb' => function($stream, $sessionId, $sessionData) use (&$sessionStore, &$newCbCalled) {
            $key = bin2hex($sessionId);
            $sessionStore[$key] = $sessionData;
            $newCbCalled = true;
        },
        'session_get_cb' => function($stream, $sessionId) use (&$sessionStore, &$getCbCalled) {
            $key = bin2hex($sessionId);
            $getCbCalled = true;
            return $sessionStore[$key] ?? null;
        },
    ]]);

    $server = stream_socket_server('tlsv1.2://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    /* Accept two connections */
    for ($i = 0; $i < 2; $i++) {
        $client = @stream_socket_accept($server, 30);
        if ($client) {
            fwrite($client, "Response " . ($i + 1) . "\n");
            fclose($client);
        }
    }

    /* Report results */
    $result = [];
    if ($newCbCalled) $result[] = "NEW_CB_CALLED";
    if ($getCbCalled) $result[] = "GET_CB_CALLED";
    $result[] = "SESSIONS:" . count($sessionStore);

    phpt_notify(message: implode(",", $result));
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

    echo "Session captured: " . ($sessionData !== null ? "YES" : "NO") . "\n";

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

    /* Get server callback results */
    $result = phpt_wait();
    echo "Server: " . trim($result) . "\n";
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_external_proper_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_external_proper.pem.tmp');
?>
--EXPECTF--
Client first connection resumed: no
Response 1
Session captured: YES
Client second connection resumed: yes
Response 2
Server: NEW_CB_CALLED,GET_CB_CALLED,SESSIONS:1
