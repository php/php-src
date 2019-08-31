--TEST--
Capture SSL session meta array in stream context for TLSv1.3
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
if (OPENSSL_VERSION_NUMBER < 0x10101000) die("skip OpenSSL v1.1.1 required");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_meta_capture_tlsv13.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_meta_capture_tlsv13-ca.pem.tmp';

$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_3_SERVER,
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    @stream_socket_accept($server, 1);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$peerName = 'session_meta_capture_tlsv13';
$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => true,
        'cafile' => '%s',
        'peer_name' => '%s',
        'capture_session_meta' => true,
    ]]);

    phpt_wait();

    stream_context_set_option($clientCtx, 'ssl', 'crypto_method', STREAM_CRYPTO_METHOD_TLSv1_3_CLIENT);
    @stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx);
    $meta = stream_context_get_options($clientCtx)['ssl']['session_meta'];
    var_dump($meta['protocol']);
CODE;
$clientCode = sprintf($clientCode, $cacertFile, $peerName);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAsFileWithKey($peerName, $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
string(7) "TLSv1.3"
