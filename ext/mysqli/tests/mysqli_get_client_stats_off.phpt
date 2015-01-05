--TEST--
mysqli_get_client_stats() - php_ini setting
--SKIPIF--
<?PHP
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_get_client_stats')) {
	die("skip only available with mysqlnd");
}
?>
--INI--
mysqlnd.collect_statistics=0
mysqlnd.collect_memory_statistics=0
--FILE--
<?php
	$before = mysqli_get_client_stats();
	if (!is_array($before) || empty($before)) {
		printf("[001] Expecting non-empty array, got %s.\n", gettype($before));
		var_dump($before);
	}

	// connect and table inc connect to mysql and create tables
	require_once('connect.inc');
	require_once('table.inc');
	$after = mysqli_get_client_stats();

	if ($before !== $after) {
		printf("[002] Statistics have changed\n");
		var_dump($before);
		var_dump($after);
	}

	foreach ($after as $k => $v)
		if ($v != 0) {
			printf("[003] Field %s should not have any other value but 0, got %s.\n",
				$k, $v);
		}

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done!