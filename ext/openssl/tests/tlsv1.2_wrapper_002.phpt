--TEST--
tlsv1.2 stream wrapper
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip");
if (OPENSSL_VERSION_NUMBER < 0x10001001) die("skip OpenSSL 1.0.1 required");
if (!function_exists('pcntl_fork')) die("skip no fork");
--FILE--
<?php
$flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
$ctx = stream_context_create(array('ssl' => array(
	'local_cert' => __DIR__ . '/streams_crypto_method.pem',
)));

$server = stream_socket_server('tlsv1.2://127.0.0.1:64321', $errno, $errstr, $flags, $ctx);
var_dump($server);

$pid = pcntl_fork();
if ($pid == -1) {
	die('could not fork');
} elseif ($pid) {
	$flags = STREAM_CLIENT_CONNECT;
	$ctx = stream_context_create(array('ssl' => array(
		'verify_peer' => false
	)));
	
	$client = stream_socket_client("tlsv1.2://127.0.0.1:64321", $errno, $errstr, 1, $flags, $ctx);
	var_dump($client);
	
	$client = @stream_socket_client("sslv3://127.0.0.1:64321", $errno, $errstr, 1, $flags, $ctx);
	var_dump($client);
	
	$client = @stream_socket_client("tlsv1.1://127.0.0.1:64321", $errno, $errstr, 1, $flags, $ctx);
	var_dump($client);
	
} else {	
	@pcntl_wait($status);
	for ($i=0; $i < 3; $i++) {
		@stream_socket_accept($server, 1);
	}
}
--EXPECTF--
resource(%d) of type (stream)
resource(%d) of type (stream)
bool(false)
bool(false)
