--TEST--
#46127, openssl_sign/verify: accept different algos 
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip, openssl required");
if (!extension_loaded("pcntl")) die("skip, pcntl required");
if (OPENSSL_VERSION_NUMBER < 0x009070af) die("skip");
?>
--FILE--
<?php

function ssl_server($port) {
	$pem = dirname(__FILE__) . '/bug46127.pem';
	$ssl = array(
			'verify_peer' => false,
			'allow_self_signed' => true,
			'local_cert' => $pem,
			//		'passphrase' => '',
		    );
	$context = stream_context_create(array('ssl' => $ssl));
	$sock = stream_socket_server('ssl://127.0.0.1:'.$port, $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context);
	if (!$sock) return false;

	$link = stream_socket_accept($sock);
	if (!$link) return false; // bad link?

	fputs($link, "Sending bug 46127\n");

	// close stuff
	fclose($link);
	fclose($sock);

	exit;
}

echo "Running bug46127\n";

$port = rand(15000, 32000);

$pid = pcntl_fork();
if ($pid == 0) { // child
	ssl_server($port);
	exit;
}

// client or failed
sleep(1);
$sock = fsockopen('ssl://127.0.0.1', $port, $errno, $errstr);
if (!$sock) exit;

echo fgets($sock);

pcntl_waitpid($pid, $status);

?>
--EXPECTF--
Running bug46127
Sending bug 46127
