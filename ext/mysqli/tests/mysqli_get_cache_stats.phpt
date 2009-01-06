--TEST--
mysqli_get_cache_stats()
--SKIPIF--
<?PHP
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_get_cache_stats')) {
	die("skip only available with mysqlnd");
}
if (ini_get("unicode.semantics")) {
	die("skip: zval cache works now only in non-unicode mode");
}
?>
--FILE--
<?php
	$tmp = $link = null;
	if (!is_null($tmp = @mysqli_get_cache_stats($link)))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	include "connect.inc";

	if (!is_array($info = mysqli_get_cache_stats()) || empty($info))
		printf("[002] Expecting array/any_non_empty, got %s/%s\n", gettype($info), $info);

	var_dump($info);

	if ($info['size'] !== $info['free_items'])
		printf("[003] Unused cache should have size (%d) == free_items (%d)\n",
			$info['size'], $info['free_items']);

	require_once('table.inc');

	if (!is_array($new_info = mysqli_get_cache_stats()) || empty($new_info))
		printf("[004] Expecting array/any_non_empty, got %s/%s\n", gettype($new_info), $new_info);

	if ($info['size'] !== $new_info['size'])
		printf("[005] Cache size should not have changes! Expecting int/%d, got %s/%d\n",
			$info['size'], gettype($new_info['size']), $new_info['size']);

	if (count($info) != count($new_info)) {
		printf("[006] Both arrays should have the same number of entries\n");
		var_dump($info);
		var_dump($new_info);
	}

	if (!$res = mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id"))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!is_object($res = mysqli_use_result($link)))
		printf("[008] Expecting object, got %s/%s. [%d] %s\n",
			gettype($res), $res, mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[009] There should be at least one row in the test table for this test, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	if (!is_array($new_info = mysqli_get_cache_stats()) || empty($new_info))
		printf("[010] Expecting array/any_non_empty, got %s/%s\n", gettype($new_info), $new_info);

	if ($new_info['get_hits'] <= $info['get_hits'])
		printf("[011] get_hits should have increased, %d (new) <= %d (old)!\n",
			$new_info['get_hits'], $info['get_hits']);

	if (!$row = mysqli_fetch_assoc($res))
		printf("[012] There should be two rows in the test table for this test, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	if (!is_array($new_info = mysqli_get_cache_stats()) || empty($new_info))
		printf("[013] Expecting array/any_non_empty, got %s/%s\n", gettype($new_info), $new_info);

	if ($new_info['put_misses'] <= $info['put_misses'])
		printf("[014] put_misses should have increased, %d (new) <= %d (old)!\n",
			$new_info['put_misses'], $info['put_misses']);

	if ($new_info['references'] < $info['references'] + 2)
		printf("[015] reference counter should have increased, %d (new) < %d + 2 (old)!\n",
			$new_info['references'], $info['references']);

	unset($row);
	mysqli_free_result($res);

	if (!is_array($info = mysqli_get_cache_stats()) || empty($info))
		printf("[016] Expecting array/any_non_empty, got %s/%s\n", gettype($info), $info);

	if ($info['free_items'] <= $new_info['free_items'])
		printf("[017] Looks like cached entries never get free'd.\n");

	mysqli_close($link);

	print "done!";
?>
--EXPECTF--
array(7) {
  [u"put_hits"]=>
  int(0)
  [u"put_misses"]=>
  int(0)
  [u"get_hits"]=>
  int(0)
  [u"get_misses"]=>
  int(0)
  [u"size"]=>
  int(%d)
  [u"free_items"]=>
  int(%d)
  [u"references"]=>
  int(%d)
}
done!
