--TEST--
TLS session resumption - num_tickets = 0 disables tickets, forces session IDs
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_no_tickets_zero.pem.tmp';

$serverCode = <<<'CODE'
    $sessionStore = [];
    $newCbCalled = 0;

    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_id_context' => 'test-no-tickets',
        'num_tickets' => 0,  // Disable ticket issuance
        'session_new_cb' => function($stream, $sessionId, $sessionData) use (&$sessionStore, &$newCbCalled) {
            $key = bin2hex($sessionId);
            $sessionStore[$key] = $sessionData;
            $newCbCalled++;
        },
        'session_get_cb' => function($stream, $sessionId) use (&$sessionStore) {
            $key = bin2hex($sessionId);
            return $sessionStore[$key] ?? null;
        },
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    // Accept two connections
    for ($i = 0; $i < 2; $i++) {
        $client = @stream_socket_accept($server, 30);
        if ($client) {
            fwrite($client, "Response " . ($i + 1) . "\n");
            usleep(50000); // Allow session storage
            fclose($client);
        }
    }

    phpt_notify(message: "NEW_CB_CALLS:$newCbCalled");
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $sessionData = null;
    $clientTickets = 0;

    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => function($stream, $sessionId, $data) use (&$sessionData, &$clientTickets) {
            $sessionData = $data;
            $clientTickets++;
        }
    ]]);

    // First connection
    $client1 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);
    if ($client1) {
        $meta1 = stream_get_meta_data($client1);
        echo "Client first connection resumed: " . ($meta1['crypto']['session_reused'] ? "yes" : "no") . "\n";
        echo trim(fgets($client1)) . "\n";
        usleep(100000); // Wait for session storage
        fclose($client1);
    }

    echo "Client received tickets on first connection: $clientTickets\n";

    // Second connection with resumption
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

    $result = phpt_wait();
    echo "Server: " . trim($result) . "\n";
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_no_tickets_zero_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_no_tickets_zero.pem.tmp');
?>
--EXPECT--
Client first connection resumed: no
Response 1
Client received tickets on first connection: 0
Client second connection resumed: no
Response 2
Server: NEW_CB_CALLS:0
