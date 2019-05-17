--TEST--
Capture SSL session meta array in stream context
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_meta_capture.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_meta_capture-ca.pem.tmp';

$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s'
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$peerName = 'session_meta_capture';
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

    stream_context_set_option($clientCtx, 'ssl', 'crypto_method', STREAM_CRYPTO_METHOD_TLSv1_0_CLIENT);
    @stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx);
    $meta = stream_context_get_options($clientCtx)['ssl']['session_meta'];
    var_dump($meta['protocol']);

    stream_context_set_option($clientCtx, 'ssl', 'crypto_method', STREAM_CRYPTO_METHOD_TLSv1_1_CLIENT);
    @stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx);
    $meta = stream_context_get_options($clientCtx)['ssl']['session_meta'];
    var_dump($meta['protocol']);

    stream_context_set_option($clientCtx, 'ssl', 'crypto_method', STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT);
    @stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx);
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
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_meta_capture.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_meta_capture-ca.pem.tmp');
?>
--EXPECT--
string(5) "TLSv1"
string(7) "TLSv1.1"
string(7) "TLSv1.2"
