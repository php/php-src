--TEST--
Server bitwise stream crypto flag assignment
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
if (OPENSSL_VERSION_NUMBER < 0x10001001) die("skip OpenSSLv1.0.1 required");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_crypto_flags_003.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_crypto_flags_003-ca.pem.tmp';

$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',

        // Only accept TLSv1.2 connections
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_0_SERVER  | STREAM_CRYPTO_METHOD_TLSv1_2_SERVER,
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$peerName = 'stream_crypto_flags_003';
$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => true,
        'cafile' => '%s',
        'peer_name' => '%s',
    ]]);

    phpt_wait();

    stream_context_set_option($clientCtx, 'ssl', 'crypto_method', STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT);
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));

    stream_context_set_option($clientCtx, 'ssl', 'crypto_method', STREAM_CRYPTO_METHOD_TLSv1_0_CLIENT);
    var_dump(@stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));

    stream_context_set_option($clientCtx, 'ssl', 'crypto_method', STREAM_CRYPTO_METHOD_TLSv1_1_CLIENT);
    var_dump(@stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));
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
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_crypto_flags_003.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_crypto_flags_003-ca.pem.tmp');
?>
--EXPECTF--
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (stream)
