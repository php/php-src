--TEST--
Bug #5547 (mysql_pconnect leaks file descriptors on reconnect)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
if (defined('PHP_WINDOWS_VERSION_MAJOR')) {
	die("skip Test doesn't work on Windows");
}
?>
--INI--
mysql.max_persistent=30
mysql.allow_persistent=1
--FILE--
<?php
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	function connect($host, $user, $passwd) {
		$conn = mysql_pconnect($host, $user, $passwd);
		if (!$conn)
			die(mysql_error()."\n");
		mysql_query("set wait_timeout=1", $conn);
		return $conn;
	}	

	$conn = connect($host, $user, $passwd);
	$opened_files = -1;
	for($i = 0; $i < 4; $i++) {
		/* wait while mysql closes connection */
		sleep(3);

		if (!mysql_ping($conn)) {
			echo "reconnect\n";
			$conn = connect($host, $user, $passwd);
		}

		$r = mysql_query('select 1', $conn);
		$error = $r ? 'OK' : mysql_error();
		if ($opened_files == -1) {
			$opened_files = trim(exec("lsof -np " . getmypid() . " | wc -l"));
			echo "OK\n";
		} else if (($tmp = trim(exec("lsof -np " . getmypid() . " | wc -l"))) != $opened_files) {
			printf("[01] [%d] different number of opened_files : expected %d, got %d", $i, $opened_files, $tmp);
		} else {
			echo "OK\n";
		}
	}


	print "done!";
?>
--EXPECTF--
Warning: mysql_ping(): MySQL server has gone away in %s on line %d
reconnect
OK

Warning: mysql_ping(): MySQL server has gone away in %s on line %d
reconnect
OK

Warning: mysql_ping(): MySQL server has gone away in %s on line %d
reconnect
OK

Warning: mysql_ping(): MySQL server has gone away in %s on line %d
reconnect
OK
done!