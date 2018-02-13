--TEST--
mysqli_num_fields()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_num_fields()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_num_fields($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	function func_test_mysqli_num_fields($link, $query, $expected, $offset, $test_free = false) {

		if (!($res = mysqli_query($link, $query))) {
			printf("[%03d] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
			return;
		}

		if ($expected !== ($tmp = mysqli_num_fields($res)))
			printf("[%03d] Expecting %s/%d, got %s/%d\n", $offset + 1,
				gettype($expected), $expected,
				gettype($tmp), $tmp);

		mysqli_free_result($res);

		if ($test_free && (false !== ($tmp = mysqli_num_fields($res))))
			printf("[%03d] Expecting false, got %s/%s\n", $offset + 2, gettype($tmp), $tmp);
	}

	func_test_mysqli_num_fields($link, "SELECT 1 AS a", 1, 5);
	func_test_mysqli_num_fields($link, "SELECT id, label FROM test", 2, 10);
	func_test_mysqli_num_fields($link, "SELECT 1 AS a, NULL AS b, 'foo' AS c", 3, 15);
	func_test_mysqli_num_fields($link, "SELECT id FROM test", 1, 20, true);

	mysqli_close($link);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_num_fields(): Couldn't fetch mysqli_result in %s on line %d
done!
