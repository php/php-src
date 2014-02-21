--TEST--
Specific crypto method for ssl:// transports.
--SKIPIF--
<?php
if (!extension_loaded('openssl')) die('skip, openssl required');
if (!extension_loaded('pcntl')) die('skip, pcntl required');
?>
--FILE--
<?php
$serverCtx = stream_context_create(['ssl' => [
	'local_cert' => dirname(__FILE__) . '/streams_crypto_method.pem',
]]);
$serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
$server = stream_socket_server('sslv3://127.0.0.1:12345', $errno, $errstr, $serverFlags, $serverCtx);

$pid = pcntl_fork();

if ($pid == -1) {
	die('could not fork');
} else if ($pid) {
	$clientCtx = stream_context_create(['ssl' => [
		'crypto_method' => STREAM_CRYPTO_METHOD_SSLv3_CLIENT,
		'verify_peer' => false,
		'verify_host' => false
	]]);

	$fp = fopen('https://127.0.0.1:12345/', 'r', false, $clientCtx);

	if ($fp) {
		fpassthru($fp);
	fclose($fp);
	}
} else {
	@pcntl_wait($status);
	$client = @stream_socket_accept($server);
	if ($client) {
		$in = '';
		while (!preg_match('/\r?\n\r?\n/', $in)) {
			$in .= fread($client, 2048);
		}
		$response = <<<EOS
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 13
Connection: close

Hello World!

EOS;
		fwrite($client, $response);
		fclose($client);

		exit();
	}
}
?>
--EXPECTF--
Hello World!
