--TEST--
TLS session resumption - warning when session_new_cb without session_id_context and verify_peer enabled
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_no_context_verify.pem.tmp';
$caCertFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_no_context_ca.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;

    /* session_new_cb without session_id_context, with verify_peer - should warn */
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'verify_peer' => true,
        'cafile' => '%s',
        'session_new_cb' => function($stream, $session) {
            echo "Callback might not be called\n";
        }
        /* Missing: 'session_id_context' => 'myapp' */
    ]]);

    $server = @stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    try {
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
$serverCode = sprintf($serverCode, $certFile, $caCertFile);

$clientCode = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;

    /* Try to use corrupted session data */
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
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
$certificateGenerator->saveCaCert($caCertFile);
$certificateGenerator->saveNewCertAsFileWithKey('session_verify_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_no_context_verify.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_no_context_ca.pem.tmp');
?>
--EXPECT--
Connection failed as expected
SERVER_EXCEPTION: session_id_context must be set if session_new_cb is set
