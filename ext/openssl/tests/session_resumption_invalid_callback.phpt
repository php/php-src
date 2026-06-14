--TEST--
TLS session resumption - invalid callback throws TypeError
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_invalid_cb.pem.tmp';

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
    $flags = STREAM_CLIENT_CONNECT;

    /* Try to use invalid callback */
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => 'not_a_valid_function',
    ]]);

    try {
        $client = @stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);
        echo "Should not reach here\n";
    } catch (TypeError $e) {
        echo "TypeError caught: " . (strpos($e->getMessage(), 'session_new_cb must be a valid callback') !== false ? "YES" : "NO");
        echo "\n";
    }
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_invalid_cb_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_invalid_cb.pem.tmp');
?>
--EXPECTF--
TypeError caught: YES
