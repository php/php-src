--TEST--
mysqli_fetch_assoc()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	// Note: no SQL type tests, internally the same function gets used as for mysqli_fetch_array() which does a lot of SQL type test

	if (!is_null($tmp = @mysqli_fetch_assoc()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_fetch_assoc($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');
	if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id LIMIT 1")) {
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	print "[005]\n";
	var_dump(mysqli_fetch_assoc($res));

	print "[006]\n";
	var_dump(mysqli_fetch_assoc($res));

	mysqli_free_result($res);

	if (!$res = mysqli_query($link, "SELECT
		1 AS a,
		2 AS a,
		3 AS c,
		4 AS C,
		NULL AS d,
		true AS e,
		5 AS '-1',
		6 AS '-10',
		7 AS '-100',
		8 AS '-1000',
		9 AS '10000',
		'a' AS '100000',
		'b' AS '1000000',
		'c' AS '9',
		'd' AS '9',
		'e' AS '01',
		'f' AS '-02'
	")) {
		printf("[007] Cannot run query, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	print "[008]\n";
	var_dump(mysqli_fetch_assoc($res));

	mysqli_free_result($res);

	if (NULL !== ($tmp = mysqli_fetch_assoc($res)))
		printf("[008] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	print "done!";
?>
--EXPECTF--
[005]
array(2) {
  [u"id"]=>
  unicode(1) "1"
  [u"label"]=>
  unicode(1) "a"
}
[006]
NULL
[008]
array(15) {
  [u"a"]=>
  unicode(1) "2"
  [u"c"]=>
  unicode(1) "3"
  [u"C"]=>
  unicode(1) "4"
  [u"d"]=>
  NULL
  [u"e"]=>
  unicode(1) "1"
  [-1]=>
  unicode(1) "5"
  [-10]=>
  unicode(1) "6"
  [-100]=>
  unicode(1) "7"
  [-1000]=>
  unicode(1) "8"
  [10000]=>
  unicode(1) "9"
  [100000]=>
  unicode(1) "a"
  [1000000]=>
  unicode(1) "b"
  [9]=>
  unicode(1) "d"
  [u"01"]=>
  unicode(1) "e"
  [u"-02"]=>
  unicode(1) "f"
}

Warning: mysqli_fetch_assoc(): Couldn't fetch mysqli_result in %s on line %d
done!
