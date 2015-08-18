--TEST--
Bug #70198 Checking liveness does not work as expected
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

/* What is checked here is 
	- start a server and listen
	- as soon as client connects, close connection and exit
	- on the client side - sleep(1) and check feof()
*/

$srv_addr = "tcp://127.0.0.1:8964";
$srv_fl = dirname(__FILE__) . "/bug70198_svr_" . md5(uniqid()) . ".php";
$srv_fl_cont = <<<SRV
<?php
\$socket = stream_socket_server('$srv_addr', \$errno, \$errstr);

if (!\$socket) {
	echo "\$errstr (\$errno)\\n";
} else {
	if (\$conn = stream_socket_accept(\$socket, 3)) {
		sleep(1);
		/* just close the connection immediately after accepting,
			the client side will need wait a bit longer to realize it.*/
		fclose(\$conn);
	}
	fclose(\$socket);
}
SRV;
file_put_contents($srv_fl, $srv_fl_cont);
$dummy0 = $dummy1 = array();
$srv_proc = proc_open(PHP_BINARY . " -n $srv_fl", $dummy0, $dummy1);

$i = 0;
/* wait a bit for the server startup */
sleep(1);
$fp = stream_socket_client($srv_addr, $errno, $errstr, 2);
if (!$fp) {
	echo "$errstr ($errno)\n";
} else {
	stream_set_blocking($fp, 0);
	sleep(2);
	while (!feof($fp)) {
		++$i;
	}
	fclose($fp);
	var_dump($i);
}

proc_close($srv_proc);
unlink($srv_fl);
?>
==DONE==
--EXPECTF--
int(0)
==DONE==

