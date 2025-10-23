--TEST--
TLS session resumption - callbacks rejected on persistent streams
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_persistent.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    $client = @stream_socket_accept($server, 30);
    if ($client) {
        fclose($client);
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT | STREAM_CLIENT_PERSISTENT;

    /* Try to use callback with persistent stream */
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => function($stream, $sessionId, $sessionData) {
            echo "This should never be called\n";
        }
    ]]);

    $client = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);

    if ($client === false) {
        echo "Connection failed as expected with persistent stream\n";
    }
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_persistent_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_persistent.pem.tmp');
?>
--EXPECTF--

Warning: stream_socket_client(): session_new_cb is not supported for persistent streams in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): Unable to connect to %s in %s on line %d
Connection failed as expected with persistent stream
