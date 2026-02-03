--TEST--
TLS session resumption - warning when trying to enable tickets with session_get_cb
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_no_ticket.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;

    /* Trying to enable tickets with external cache - should warn */
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_id_context' => 'test-app',
        'no_ticket' => false,  // Explicitly trying to enable tickets
        'session_new_cb' => function($stream, $sessionData) {
            // Store session
        },
        'session_get_cb' => function($stream, $sessionId) {
            return null;
        }
    ]]);

    try {
        $server = @stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
        phpt_notify_server_start($server);

        $client = @stream_socket_accept($server, 30);
        if ($client === false) {
            phpt_notify(message: "SERVER_FAILED_UNEXPECTEDLY");
        } else {
            phpt_notify(message: "SERVER_CREATED_UNEXPECTEDLY");
            fclose($server);
        }
    } catch (\Throwable $e) {
        phpt_notify(message: "SERVER_EXCEPTION: " . $e->getMessage());
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;

    /* Try to use corrupted session data */
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_data' => 'this_is_invalid_session_data',
    ]]);

    $client = @stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);

    if ($client === false) {
        echo "Connection failed as expected\n";
    }

    $result = phpt_wait();
    echo trim($result) . "\n";
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_no_ticket_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_no_ticket.pem.tmp');
?>
--EXPECT--
SERVER_EXCEPTION: Session tickets cannot be enabled when session_get_cb is setConnection failed as expected

