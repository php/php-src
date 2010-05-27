--TEST--
mysqli_get_cache_stats() - disabled via php.ini
--INI--
mysqlnd.collect_statistics="0"
mysqlnd.collect_memory_statistics="0"
--SKIPIF--
<?PHP
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_get_cache_stats')) {
	die("skip only available with mysqlnd");
}
?>
--FILE--
<?php
	$before = mysqli_get_cache_stats();
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

	require_once('table.inc');
	if (!$res = mysqli_query($link, "SELECT id, label FROM test")) {
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	while ($row = mysqli_fetch_assoc($res))
		;
	if (!$res = mysqli_query($link, "SELECT id, label FROM test")) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	while ($row = mysqli_fetch_assoc($res))
		;

	$after = mysqli_get_cache_stats();
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
