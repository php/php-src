--TEST--
Bug #65729: CN_match gives false positive when wildcard is used
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip");
if (!function_exists('pcntl_fork')) die("skip no fork");
--FILE--
<?php
$context = stream_context_create();

stream_context_set_option($context, 'ssl', 'local_cert', __DIR__ . "/bug65729.pem");
stream_context_set_option($context, 'ssl', 'allow_self_signed', true);
$server = stream_socket_server('ssl://127.0.0.1:64321', $errno, $errstr,
	STREAM_SERVER_BIND|STREAM_SERVER_LISTEN, $context);

$expected_names = array('foo.test.com.sg', 'foo.test.com', 'FOO.TEST.COM', 'foo.bar.test.com');

$pid = pcntl_fork();
if ($pid == -1) {
	die('could not fork');
} else if ($pid) {
	foreach ($expected_names as $expected_name) {
		$contextC = stream_context_create(array(
			'ssl' => array(
				'verify_peer'		=> true,
				'allow_self_signed'	=> true,
				'CN_match'		=> $expected_name,
			)
		));
		var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1,
		STREAM_CLIENT_CONNECT, $contextC));
	}
} else {	
	@pcntl_wait($status);
	foreach ($expected_names as $name) {
		@stream_socket_accept($server, 1);
	}
}
--EXPECTF--
Warning: stream_socket_client(): Peer certificate CN=`*.test.com' did not match expected CN=`foo.test.com.sg' in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to ssl://127.0.0.1:64321 (Unknown error) in %s on line %d
bool(false)
resource(%d) of type (stream)
resource(%d) of type (stream)

Warning: stream_socket_client(): Peer certificate CN=`*.test.com' did not match expected CN=`foo.bar.test.com' in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to ssl://127.0.0.1:64321 (Unknown error) in %s on line %d
bool(false)
