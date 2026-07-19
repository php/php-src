--TEST--
TLS 1.3 early data with an invalid early_data option raises TypeError
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (!defined('STREAM_CRYPTO_METHOD_TLSv1_3_SERVER')) die("skip TLS 1.3 not available");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_early_data_invalid.pem.tmp';

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);
    @stream_socket_accept($server, 3);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'early_data' => 123,
    ]]);
    try {
        @stream_socket_client('tls://{{ ADDR }}', $errno, $errstr,
            5, STREAM_CLIENT_CONNECT, $ctx);
        echo "no exception\n";
    } catch (TypeError $e) {
        echo "caught: ", $e->getMessage(), "\n";
    }
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('tls_early_data_invalid', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_early_data_invalid.pem.tmp');
?>
--EXPECT--
caught: early_data must be a string
