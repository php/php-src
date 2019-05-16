--TEST--
Testing peer fingerprint on connection
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'openssl_peer_fingerprint_basic.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'openssl_peer_fingerprint_basic-ca.pem.tmp';

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
CODE;
$serverCode = sprintf($serverCode, $certFile);

$peerName = 'openssl_peer_fingerprint_basic';
$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer'       => true,
        'cafile'            => '%s',
        'capture_peer_cert' => true,
        'peer_name'         => '%s',
    ]]);

    phpt_wait();

    stream_context_set_option($clientCtx, 'ssl', 'peer_fingerprint', '%s');
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx));

    stream_context_set_option($clientCtx, 'ssl', 'peer_fingerprint', [
        'sha256' => '%s',
    ]);
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx));
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAsFileWithKey($peerName, $certFile);

$actualMd5 = $certificateGenerator->getCertDigest('md5');
$lastCharacter = substr($actualMd5, -1, 1);
$brokenLastCharacter = dechex(hexdec($lastCharacter) ^ 1);
$brokenMd5 = substr($actualMd5, 0, -1) . $brokenLastCharacter;
$actualSha256 = $certificateGenerator->getCertDigest('sha256');

$clientCode = sprintf($clientCode, $cacertFile, $peerName, $brokenMd5, $actualSha256);


include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'openssl_peer_fingerprint_basic.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'openssl_peer_fingerprint_basic-ca.pem.tmp');
?>
--EXPECTF--
Warning: stream_socket_client(): peer_fingerprint match failure in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to ssl://127.0.0.1:64321 (Unknown error) in %s on line %d
bool(false)
resource(%d) of type (stream)
