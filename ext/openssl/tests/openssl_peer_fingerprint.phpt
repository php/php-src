--TEST--
Testing peer fingerprint on connection
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip");
if (!function_exists('pcntl_fork')) die("skip no fork");
--FILE--
<?php
$context = stream_context_create();

stream_context_set_option($context, 'ssl', 'local_cert', __DIR__ . "/bug54992.pem");
stream_context_set_option($context, 'ssl', 'allow_self_signed', true);
$server = stream_socket_server('ssl://127.0.0.1:64321', $errno, $errstr,
	STREAM_SERVER_BIND|STREAM_SERVER_LISTEN, $context);


$pid = pcntl_fork();
if ($pid == -1) {
	die('could not fork');
} else if ($pid) {
	$contextC = stream_context_create(
		array(
			'ssl' => array(
				'verify_peer'		=> true,
				'cafile'		=> __DIR__ . '/bug54992-ca.pem',
				'capture_peer_cert'	=> true,
				'CN_match'		=> 'bug54992.local',
				'peer_fingerprint'	=> '81cafc260aa8d82956ebc6212a362ece',
			)
		)
	);
	// should be: 81cafc260aa8d82956ebc6212a362ecc
	var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1,
		STREAM_CLIENT_CONNECT, $contextC));

	$contextC = stream_context_create(
		array(
			'ssl' => array(
				'verify_peer'		=> true,
				'cafile'		=> __DIR__ . '/bug54992-ca.pem',
				'capture_peer_cert'	=> true,
				'CN_match'		=> 'bug54992.local',
				'peer_fingerprint'	=> array(
					'sha256' => '78ea579f2c3b439359dec5dac9d445108772927427c4780037e87df3799a0aa0',
				),
			)
		)
	);

	var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1,
		STREAM_CLIENT_CONNECT, $contextC));
} else {
	@pcntl_wait($status);
	@stream_socket_accept($server, 1);
	@stream_socket_accept($server, 1);
}
--EXPECTF--
Warning: stream_socket_client(): Peer fingerprint doesn't match in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to ssl://127.0.0.1:64321 (Unknown error) in %s on line %d
bool(false)
resource(%d) of type (stream)
