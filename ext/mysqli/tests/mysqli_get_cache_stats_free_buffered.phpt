--TEST--
mysqli_get_cache_stats() - freeing for buffered result sets
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
	include "connect.inc";
	require_once('table.inc');

	if (!is_array($info = mysqli_get_cache_stats()) || empty($info))
		printf("[001] Expecting array/any_non_empty, got %s/%s\n", gettype($info), $info);

	if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id"))
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$rows = array();
	while ($rows[] = mysqli_fetch_assoc($res))
		;
	mysqli_free_result($res);

	if (!is_array($new_info = mysqli_get_cache_stats()) || empty($new_info))
		printf("[003] Expecting array/any_non_empty, got %s/%s\n", gettype($new_info), $new_info);

	if ($new_info['free_items'] >= $info['free_items']) {
		printf("[004] mysqli_free_result() should not yet have free slots again, because \$rows still exists\n");
		var_dump($info);
			var_dump($new_info);
	}

	/* nothing should change because GC will happen some time after free_result */
	unset($rows);

	if (!is_array($new_info = mysqli_get_cache_stats()) || empty($new_info))
		printf("[005] Expecting array/any_non_empty, got %s/%s\n", gettype($new_info), $new_info);

	if (defined("WE_HAVE_GARBAGE_COLLECTOR_TO_FREE_AFTER_ON_UNSET_AFTER_FREE_RESULT")) {
		/*
			For now we can't reclaim the slots after the free_result is called.
			unset() should happen before free_result().
		*/
		if ($new_info['free_items'] < $info['free_items']) {
			printf("[006] \$rows has been unset, free item count should be back to the original value\n");
			var_dump($info);
			var_dump($new_info);
		}
	} else {
		/* We have to reset $info */
		$info = mysqli_get_cache_stats();
	}

	if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id"))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$rows = array();
	while ($rows[] = mysqli_fetch_assoc($res))
		;
	unset($rows);
	mysqli_free_result($res);

	if (!is_array($new_info = mysqli_get_cache_stats()) || empty($new_info))
		printf("[008] Expecting array/any_non_empty, got %s/%s\n", gettype($new_info), $new_info);

	if ($new_info['free_items'] < $info['free_items']) {
		printf("[009] \$rows has been unset, mysqli_free_result() has been called, free item count should be back to the original value\n");
		var_dump($info);
		var_dump($new_info);
	}

	mysqli_close($link);

	if (!is_array($new_info = mysqli_get_cache_stats()) || empty($new_info))
		printf("[010] Expecting array/any_non_empty, got %s/%s\n", gettype($new_info), $new_info);

	if ($new_info['free_items'] < $info['free_items']) {
		printf("[011] connection has been closed, free item count should be back to the original value\n");
		var_dump($info);
		var_dump($new_info);
	}

	print "done!";
?>
--EXPECTF--
done!