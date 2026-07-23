--TEST--
TLS session resumption - num_tickets controls ticket generation (TLS 1.3)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_num_tickets.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    
    // Test with num_tickets = 3 (should issue 3 tickets)
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_3_SERVER,
        'num_tickets' => 3,  // Issue 3 tickets per connection
    ]]);

    $server = stream_socket_server('tlsv1.3://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    // Accept one connection
    $client = @stream_socket_accept($server, 30);
    if ($client) {
        fwrite($client, "Ticket test\n");
        // Keep connection open briefly to allow tickets to be sent
        usleep(100000); // 100ms
        fclose($client);
    }
    
    phpt_notify(message: "SERVER_DONE");
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ticketCount = 0;

    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_3_CLIENT,
        'session_new_cb' => function($stream, $session) use (&$ticketCount) {
            $ticketCount++;
        }
    ]]);

    $client = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);
    if ($client) {
        $response = fgets($client);
        echo trim($response) . "\n";
        
        // Keep connection open briefly to receive all tickets
        usleep(150000); // 150ms
        fclose($client);
    }

    echo "Tickets received: $ticketCount\n";
    
    $result = phpt_wait();
    echo trim($result) . "\n";
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_num_tickets_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_num_tickets.pem.tmp');
?>
--EXPECTF--
Ticket test
Tickets received: 3
SERVER_DONE
