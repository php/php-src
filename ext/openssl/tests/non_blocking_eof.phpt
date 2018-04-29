--TEST--
php_stream_eof() should not block on SSL non-blocking streams when packets are fragmented
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php

$clientCode = <<<'CODE'
	$context = stream_context_create(['ssl' => ['verify_peer' => false, 'peer_name' => 'bug54992.local']]);

	phpt_wait('server');
	phpt_notify('proxy');
	
	phpt_wait('proxy');
	$fp = stream_socket_client("ssl://127.0.0.1:10012", $errornum, $errorstr, 3000, STREAM_CLIENT_CONNECT, $context);
	stream_set_blocking($fp, false);

	$read = [$fp];
	$buf = '';
	while (stream_select($read, $write, $except, 1000)) {
		$chunk = stream_get_contents($fp, 4096);
		var_dump($chunk);
		$buf .= $chunk;
		if ($buf === 'hello, world') {
			break;
		}
	}
	
	phpt_notify('server');
	phpt_notify('proxy');
CODE;

$serverCode = <<<'CODE'
	$context = stream_context_create(['ssl' => ['local_cert' => __DIR__ . '/bug54992.pem']]);

	$flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
	$fp = stream_socket_server("ssl://127.0.0.1:10011", $errornum, $errorstr, $flags, $context);
	phpt_notify();
	
	$conn = stream_socket_accept($fp);
	fwrite($conn, 'hello, world');
	
	phpt_wait();
	fclose($conn);
CODE;

$proxyCode = <<<'CODE'
	phpt_wait();

	$upstream = stream_socket_client("tcp://127.0.0.1:10011", $errornum, $errorstr, 3000, STREAM_CLIENT_CONNECT);
	stream_set_blocking($upstream, false);

	$flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
	$server = stream_socket_server("tcp://127.0.0.1:10012", $errornum, $errorstr, $flags);
	phpt_notify();
	
	$conn = stream_socket_accept($server);
	stream_set_blocking($conn, false);
	
	$read = [$upstream, $conn];
	while (stream_select($read, $write, $except, 1)) {
		foreach ($read as $fp) {
			$data = stream_get_contents($fp);
			if ($fp === $conn) {
				fwrite($upstream, $data);
			} else {
				if ($data !== '' && $data[0] === chr(23)) {
					$parts = str_split($data, (int) ceil(strlen($data) / 3));
					foreach ($parts as $part) {
						fwrite($conn, $part);
						usleep(1000);
					}
				} else {
					fwrite($conn, $data);
				}
			}
		}
		if (feof($upstream)) {
			break;
		}
		$read = [$upstream, $conn];
	}
	
	phpt_wait();
CODE;

include 'ServerClientProxyTestCase.inc';
ServerClientProxyTestCase::getInstance()->run($clientCode, [
	'server' => $serverCode,
	'proxy' => $proxyCode,
]);
?>
--EXPECT--
string(0) ""
string(0) ""
string(12) "hello, world"
