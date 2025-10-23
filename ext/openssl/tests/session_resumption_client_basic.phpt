--TEST--
TLS session resumption - client basic resumption
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_client.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    /* Accept two connections */
    for ($i = 0; $i < 2; $i++) {
        $client = @stream_socket_accept($server, 30);
        if ($client) {
            fwrite($client, "Hello from server\n");
            fclose($client);
        }
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $sessionData = '';
    $sessionReceived = false;

    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => function($stream, $sessionId, $sessionDataArg) use (&$sessionReceived, &$sessionData) {
            $sessionData = $sessionDataArg;
            $sessionReceived = true;
        }
    ]]);

    /* First connection - full handshake */
    $client1 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);
    if ($client1) {
        $response = fgets($client1);
        echo "First connection: " . trim($response) . "\n";
        fclose($client1);
    }

    var_dump($sessionReceived);
    var_dump(strlen($sessionData) > 0);

    /* Second connection - resumed session */
    $ctx2 = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_data' => $sessionData,
    ]]);

    $client2 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx2);
    if ($client2) {
        $response = fgets($client2);
        echo "Second connection: " . trim($response) . "\n";
        fclose($client2);
    }

    echo "Session resumption test completed\n";
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_resumption_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_client.pem.tmp');
?>
--EXPECTF--
First connection: Hello from server
bool(true)
bool(true)
Second connection: Hello from server
Session resumption test completed
