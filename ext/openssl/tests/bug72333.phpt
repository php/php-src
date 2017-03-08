--TEST--
Bug #72333: fwrite() on non-blocking SSL sockets doesn't work
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$serverCode = <<<'CODE'
	$context = stream_context_create(['ssl' => ['local_cert' => __DIR__ . '/bug54992.pem']]);

	$flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
	$fp = stream_socket_server("ssl://127.0.0.1:10011", $errornum, $errorstr, $flags, $context);
	phpt_notify();
	$conn = stream_socket_accept($fp);

	for ($i = 0; $i < 5; $i++) {
		fread($conn, 100000);
		usleep(20000);
	}
CODE;

$clientCode = <<<'CODE'
	$context = stream_context_create(['ssl' => ['verify_peer' => false, 'peer_name' => 'bug54992.local']]);

	phpt_wait();
	$fp = stream_socket_client("ssl://127.0.0.1:10011", $errornum, $errorstr, 3000, STREAM_CLIENT_CONNECT, $context);
	stream_set_blocking($fp, 0);

	function blocking_fwrite($fp, $buf) {
		$write = [$fp];
		$total = 0;
		while (stream_select($read, $write, $except, 180)) {
			$result = fwrite($fp, $buf);
			$total += $result;
			if ($total >= strlen($buf)) {
				return $total;
			}
			$buf = substr($buf, $total);
		}
	}

	$str1 = str_repeat("a", 5000000);
	blocking_fwrite($fp, $str1);
	echo "done";
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
done

