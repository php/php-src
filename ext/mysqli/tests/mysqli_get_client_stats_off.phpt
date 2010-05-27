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

	/*
	NOTE: the function belongs to the mysqnd zval cache. The
	mysqlnd zval cache was part of PHP from PHP 5.3.0(-dev) to
	PHP 5.3.0RC3 or something. And it was turned off by default.
	The function never returned anything meaningful in any released version of PHP.

	*/
	if (!is_array($before)) {
		printf("[001] Expecting array, got %s.\n", gettype($before));
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

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done!