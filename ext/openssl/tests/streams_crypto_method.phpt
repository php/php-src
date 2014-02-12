--TEST--
Specific crypto method for ssl:// transports.
--SKIPIF--
<?php 
if (!extension_loaded('openssl')) die('skip, openssl required');
if (!extension_loaded('pcntl')) die('skip, pcntl required');
?>
--FILE--
<?php
function client($port, $method) {
	$ctx = stream_context_create();
	stream_context_set_option($ctx, 'ssl', 'crypto_method', $method);
	stream_context_set_option($ctx, 'ssl', 'verify_peer', false);

	$fp = @fopen('https://127.0.0.1:' . $port . '/', 'r', false, $ctx);
	if ($fp) {
		fpassthru($fp);
		fclose($fp);
	}
}

function server($port, $transport) {
	$context = stream_context_create();

	stream_context_set_option($context, 'ssl', 'local_cert', dirname(__FILE__) . '/streams_crypto_method.pem');
	stream_context_set_option($context, 'ssl', 'allow_self_signed', true);
	stream_context_set_option($context, 'ssl', 'verify_peer', false);

	$server = stream_socket_server($transport . '127.0.0.1:' . $port, $errno, $errstr, STREAM_SERVER_BIND|STREAM_SERVER_LISTEN, $context);

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

$port1 = rand(15000, 16000);
$port2 = rand(16001, 17000);

$pid1 = pcntl_fork();
$pid2 = pcntl_fork();

if ($pid1 == 0 && $pid2 != 0) {
	server($port1, 'sslv3://');
	exit;
}

if ($pid1 != 0 && $pid2 == 0) {
	server($port2, 'sslv3://');
	exit;
}

client($port1, STREAM_CRYPTO_METHOD_SSLv3_CLIENT);
client($port2, STREAM_CRYPTO_METHOD_SSLv2_CLIENT);

pcntl_waitpid($pid1, $status);
pcntl_waitpid($pid2, $status);
?>
--EXPECTF--
Hello World!
