--TEST--
Bug #65538 SSL context "cafile" supports phar wrapper
--SKIPIF--
<?php
if (!extension_loaded('openssl')) die('skip, openssl required');
if (!extension_loaded('pcntl')) die('skip, pcntl required');
if (!extension_loaded('phar')) die('skip, phar required');
?>
--FILE--
<?php
$serverCtx = stream_context_create(['ssl' => [
	'local_cert' => __DIR__ . '/bug54992.pem'
]]);
$serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
$server = stream_socket_server('ssl://127.0.0.1:64321', $errno, $errstr, $serverFlags, $serverCtx);

$pid = pcntl_fork();

if ($pid == -1) {
	die('could not fork');
} else if ($pid) {
	$clientCtx = stream_context_create(['ssl' => [
			'cafile' => 'phar://' . __DIR__ . '/bug65538.phar/bug54992-ca.pem',
			'CN_match' => 'bug54992.local'
	]]);
	$html = file_get_contents('https://127.0.0.1:64321', false, $clientCtx);
	var_dump($html);
} else {
	@pcntl_wait($status);

	$client = @stream_socket_accept($server);
	if ($client) {
		$in = '';
		while (!preg_match('/\r?\n\r?\n/', $in)) {
			$in .= fread($client, 2048);
		}
		$response = <<<EOS
HTTP/1.0 200 OK
Content-Type: text/plain
Content-Length: 12
Connection: close

Hello World!
EOS;

		fwrite($client, $response);
		fclose($client);
	}
}
?>
--EXPECTF--
string(12) "Hello World!"
