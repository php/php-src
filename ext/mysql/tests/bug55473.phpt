--TEST--
Bug #5547 (mysql_pconnect leaks file descriptors on reconnect)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
if (defined('PHP_WINDOWS_VERSION_MAJOR')) {
	die("skip Test doesn't work on Windows");
}

if (!($output = @exec("lsof -np " . getmypid())))
	die("skip Test can't find command line tool lsof");
?>
--INI--
mysql.max_persistent=30
mysql.allow_persistent=1
--FILE--
<?php
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

 	if ($socket)
        $host = sprintf("%s:%s", $host, $socket);
    else if ($port)
        $host = sprintf("%s:%s", $host, $port);

	function connect($host, $user, $passwd) {
		$conn = mysql_pconnect($host, $user, $passwd);

		if (!$conn)
			die(sprintf("[001] %s\n", mysql_error()));

		if (!mysql_query("set wait_timeout=1", $conn))
			printf("[002] [%d] %s\n", mysql_errno($conn), mysql_error($conn));

		return $conn;
	}	

	$conn = connect($host, $user, $passwd);
	$opened_files = -1;

	for ($i = 0; $i < 4; $i++) {
		/* wait while mysql closes connection */
		sleep(3);

		if (!mysql_ping($conn)) {
			printf("[003] reconnect %d\n", $i);
			$conn = connect($host, $user, $passwd);  
		}

		$r = mysql_query('select 1', $conn);
		if (!$r)
			printf("[004] [%d] %s\n", mysql_errno($conn), mysql_error($conn));


		if ($opened_files == -1) {
			$opened_files = trim(exec("lsof -np " . getmypid() . " | wc -l"));
			printf("[005] Setting openened files...\n");
		} else if (($tmp = trim(exec("lsof -np " . getmypid() . " | wc -l"))) != $opened_files) {
			printf("[006] [%d] different number of opened_files : expected %d, got %d", $i, $opened_files, $tmp);
		} else {
			printf("[007] Opened files as expected\n");
		}
	}

	print "done!";
?>
--EXPECTF--
Deprecated: mysql_pconnect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Warning: mysql_ping(): MySQL server has gone away in %s on line %d
[003] reconnect 0

Deprecated: mysql_pconnect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
[005] Setting openened files...

Warning: mysql_ping(): MySQL server has gone away in %s on line %d
[003] reconnect 1

Deprecated: mysql_pconnect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
[007] Opened files as expected

Warning: mysql_ping(): MySQL server has gone away in %s on line %d
[003] reconnect 2

Deprecated: mysql_pconnect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
[007] Opened files as expected

Warning: mysql_ping(): MySQL server has gone away in %s on line %d
[003] reconnect 3

Deprecated: mysql_pconnect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
[007] Opened files as expected
done!
