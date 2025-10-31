--TEST--
TLS session resumption - server external cache callbacks with no verify
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_server.pem.tmp';

$serverCode = <<<'CODE'
    $sessionStore = [];
    $newCbCalled = false;
    $getCbCalled = false;
    $removeCbCalled = false;

    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'verify_peer' => false,
        'no_ticket' => true,
        'session_cache' => true,
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
        'session_remove_cb' => function($stream, $sessionId) use (&$sessionStore, &$removeCbCalled) {
            $key = bin2hex($sessionId);
            unset($sessionStore[$key]);
            $removeCbCalled = true;
        }
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    /* Accept two connections */
    for ($i = 0; $i < 2; $i++) {
        $client = @stream_socket_accept($server, 30);
        if ($client) {
            fwrite($client, "Response " . ($i + 1) . "\n");
            fclose($client);
        }
    }

    /* Notify client about callback invocations */
    $result = [];
    if ($newCbCalled) $result[] = "NEW_CB_CALLED";
    if ($getCbCalled) $result[] = "GET_CB_CALLED";
    if ($removeCbCalled) $result[] = "REMOVE_CB_CALLED";

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
        echo trim(fgets($client1)) . "\n";
        fclose($client1);
    }

    /* Second connection with session resumption */
    $ctx2 = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_data' => $sessionData,
    ]]);

    $client2 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx2);
    if ($client2) {
        echo trim(fgets($client2)) . "\n";
        fclose($client2);
    }

    /* Get server callback results */
    $result = phpt_wait();
    echo "Server callbacks: " . trim($result) . "\n";
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_server_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_server.pem.tmp');
?>
--EXPECTF--
Response 1
Response 2
Server callbacks: NEW_CB_CALLED,GET_CB_CALLED
