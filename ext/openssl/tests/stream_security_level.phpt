--TEST--
security_level setting to prohibit cert
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (OPENSSL_VERSION_NUMBER < 0x10100000) die("skip OpenSSL >= v1.1.0 required");
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
// https://www.openssl.org/docs/man1.1.0/ssl/SSL_CTX_get_security_level.html
$securityLevel = 2;

// Security level 2 refuses certs signed by keys with length of less than 2048 bits
$keyLength = 1024;

$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_security_level.pem.tmp';
$cacertFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_security_level-ca.pem.tmp';

$serverCode = <<<'CODE'
	$serverUri = "ssl://127.0.0.1:64322";
	$serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
	$serverCtx = stream_context_create(['ssl' => [
		'local_cert' => '%s',
		// Make sure the server side starts up successfully if the default security level is
		// higher. We want to test the error at the client side.
		'security_level' => 1,
	]]);

	$server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
	phpt_notify();

	@stream_socket_accept($server, 1);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
	$serverUri = "ssl://127.0.0.1:64322";
	$clientFlags = STREAM_CLIENT_CONNECT;
	$clientCtx = stream_context_create(['ssl' => [
		'security_level' => %d,
		'verify_peer' => true,
		'cafile' => '%s',
		'verify_peer_name' => false
	]]);

	phpt_wait();
	$client = stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx);

	var_dump($client);
CODE;
$clientCode = sprintf($clientCode, $securityLevel, $cacertFile);

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAsFileWithKey('stream_security_level', $certFile, $keyLength);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_security_level.pem.tmp');
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_security_level-ca.pem.tmp');
?>
--EXPECTF--
Warning: stream_socket_client(): SSL operation failed with code 1. OpenSSL Error messages:
error:1416F086:SSL routines:tls_process_server_certificate:certificate verify failed in %s : eval()'d code on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s : eval()'d code on line %d

Warning: stream_socket_client(): unable to connect to ssl://127.0.0.1:64322 (Unknown error) in %s : eval()'d code on line %d
bool(false)
