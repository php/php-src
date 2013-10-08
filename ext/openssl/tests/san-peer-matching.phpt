--TEST--
Peer verification matches SAN names
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip");
if (!function_exists('pcntl_fork')) die("skip no fork");
--FILE--
<?php
$context = stream_context_create(array(
	'ssl' => array(
		'local_cert' => __DIR__ . '/san-cert.pem',
		'allow_self_signed' => true,
	),
));

$server = stream_socket_server('ssl://127.0.0.1:64321', $errno, $errstr,
	STREAM_SERVER_BIND|STREAM_SERVER_LISTEN, $context);


$pid = pcntl_fork();
if ($pid == -1) {
	die('could not fork');
} else if ($pid) {
	$contextC = stream_context_create(
		array(
			'ssl' => array(
				'verify_peer' => true,
				'cafile' => __DIR__ . '/san-ca.pem',
				'CN_match' => 'example.org',
			)
		)
	);
	var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1,
		STREAM_CLIENT_CONNECT, $contextC));

	$contextC = stream_context_create(array(
		'ssl' => array(
			'verify_peer' => true,
			'cafile' => __DIR__ . '/san-ca.pem',
			'CN_match' => 'moar.example.org',
		)
	));

	var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1,
                STREAM_CLIENT_CONNECT, $contextC));

} else {	
	@pcntl_wait($status);
	@stream_socket_accept($server, 1);
	@stream_socket_accept($server, 1);
}
--EXPECTF--
resource(%d) of type (stream)

Warning: stream_socket_client(): Unable to locate peer certificate CN in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to ssl://127.0.0.1:64321 (Unknown error) in %s on line %d
bool(false)
