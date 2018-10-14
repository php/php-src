--TEST--
mysqli_get_connection_stats() - disable via php.ini
--INI--
mysqlnd.collect_statistics="0"
mysqlnd.collect_memory_statistics="0"
--SKIPIF--
<?PHP
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_get_connection_stats')) {
	die("skip only available with mysqlnd");
}
?>
--FILE--
<?php
	// connect and table inc connect to mysql and create tables
	require_once('connect.inc');

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
	}
	$before = mysqli_get_connection_stats($link);
	if (!is_array($before) || empty($before)) {
		printf("[002] Expecting non-empty array, got %s.\n", gettype($before));
		var_dump($before);
	}

	mysqli_close($link);
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
	}
	$after = mysqli_get_connection_stats($link);

	if ($before !== $after) {
		printf("[004] Statistics differ!");
		var_dump($before);
		var_dump($after);
	}

	foreach ($after as $k => $v)
		if ($v != 0) {
			printf("[004] Field %s should not have any other value but 0, got %s.\n",
				$k, $v);
		}

	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
done!
