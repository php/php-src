--TEST--
Peer verification enabled for client streams
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip");
if (!function_exists('pcntl_fork')) die("skip no fork");
--FILE--
<?php
$flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
$ctx = stream_context_create(['ssl' => [
	'local_cert' => __DIR__ . '/bug54992.pem',
	'allow_self_signed' => true
]]);
$server = stream_socket_server('ssl://127.0.0.1:64321', $errno, $errstr, $flags, $ctx);

$pid = pcntl_fork();
if ($pid == -1) {
	die('could not fork');
} else if ($pid) {
	// Expected to fail -- no CA File present
	var_dump(@stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1, STREAM_CLIENT_CONNECT));
	
	// Expected to fail -- no CA File present
	$ctx = stream_context_create(['ssl' => ['verify_peer' => true]]);
	var_dump(@stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $ctx));
	
	// Should succeed with peer verification disabled in context
	$ctx = stream_context_create(['ssl' => ['verify_peer' => false]]);
	var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $ctx));

	// Should succeed with CA file specified in context
	$ctx = stream_context_create(['ssl' => [
		'cafile' => __DIR__ . '/bug54992-ca.pem',
		'CN_match' => 'bug54992.local',
	]]);
	var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $ctx));

	// Should succeed with globally available CA file specified via php.ini
	$cafile = __DIR__ . '/bug54992-ca.pem';
	ini_set('openssl.cafile', $cafile);
	var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $ctx));

} else {	
	@pcntl_wait($status);
	@stream_socket_accept($server, 3);
	@stream_socket_accept($server, 3);
	@stream_socket_accept($server, 3);
	@stream_socket_accept($server, 3);
	@stream_socket_accept($server, 3);
}
--EXPECTF--
bool(false)
bool(false)
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (stream)
