--TEST--
#48182,ssl handshake fails during asynchronous socket connection
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip, openssl required");
if (!extension_loaded("pcntl")) die("skip, pcntl required");
if (OPENSSL_VERSION_NUMBER < 0x009070af) die("skip");
?>
--FILE--
<?php

function ssl_server($port) {
	$host = 'ssl://127.0.0.1'.':'.$port;
	$flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
	$data = "Sending bug48182\n";

	$pem = dirname(__FILE__) . '/bug46127.pem';
	$ssl_params = array( 'verify_peer' => false, 'allow_self_signed' => true, 'local_cert' => $pem);
	$ssl = array('ssl' => $ssl_params);

	$context = stream_context_create($ssl);
	$sock = stream_socket_server($host, $errno, $errstr, $flags, $context);
	if (!$sock) return false;

	$link = stream_socket_accept($sock);
	if (!$link) return false; // bad link?

	$r = array($link);
	$w = array();
	$e = array();
	if (stream_select($r, $w, $e, 1, 0) != 0)
		$data .= fread($link, 8192);

	$r = array();
	$w = array($link);
	if (stream_select($r, $w, $e, 1, 0) != 0)
		$wrote = fwrite($link, $data, strlen($data));

	// close stuff
	fclose($link);
	fclose($sock);

	exit;
}

function ssl_async_client($port) {
	$host = 'ssl://127.0.0.1'.':'.$port;
	$flags = STREAM_CLIENT_CONNECT | STREAM_CLIENT_ASYNC_CONNECT;
	$data = "Sending data over to SSL server in async mode with contents like Hello World\n";

	$socket = stream_socket_client($host, $errno, $errstr, 10, $flags);
	stream_set_blocking($socket, 0);

	while ($socket && $data) {
		$wrote = fwrite($socket, $data, strlen($data));
		$data = substr($data, $wrote);
	}

	$r = array($socket);
	$w = array();
	$e = array();
	if (stream_select($r, $w, $e, 1, 0) != 0) 
	{
		$data .= fread($socket, 1024);
	}

	echo "$data";

	fclose($socket);
}

echo "Running bug48182\n";

$port = rand(15000, 32000);

$pid = pcntl_fork();
if ($pid == 0) { // child
	ssl_server($port);
	exit;
}

// client or failed
sleep(1);
ssl_async_client($port);

pcntl_waitpid($pid, $status);

?>
--EXPECTF--
Running bug48182
Sending bug48182
Sending data over to SSL server in async mode with contents like Hello World
