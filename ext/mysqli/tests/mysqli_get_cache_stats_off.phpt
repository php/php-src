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
	if (!is_array($before) || empty($before)) {
		printf("[001] Expecting non-empty array, got %s.\n", gettype($before));
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
	/* references has to be maintained - it is used for memory management */
	$ignore = array('references' => true);
	foreach ($before as $k => $v) {
		if (isset($ignore[$k]))
			continue;

		if ($before[$k] != $after[$k])
			printf("[004] Statistics have changed - %s: %s => %s\n", $
				$k, $before[$k], $after[$k]);
	}

	$ignore = array("size" => true, "free_items" => true, "references" => true);
	foreach ($after as $k => $v) {
		if ($v != 0 && !isset($ignore[$k])) {
			printf("[005] Field %s should not have any other value but 0, got %s.\n",
				$k, $v);
		}
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