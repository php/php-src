--TEST--
mysqli_num_rows()
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

	if (!is_null($tmp = @mysqli_num_rows()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_num_rows($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	function func_test_mysqli_num_rows($link, $query, $expected, $offset, $test_free = false) {

		if (!$res = mysqli_query($link, $query, MYSQLI_STORE_RESULT)) {
			printf("[%03d] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
			return;
		}

		if ($expected !== ($tmp = mysqli_num_rows($res)))
			printf("[%03d] Expecting %s/%d, got %s/%d\n", $offset + 1,
				gettype($expected), $expected,
				gettype($tmp), $tmp);

		mysqli_free_result($res);

		if ($test_free && (NULL !== ($tmp = mysqli_num_rows($res))))
			printf("[%03d] Expecting NULL, got %s/%s\n", $offset + 2, gettype($tmp), $tmp);

	}

	func_test_mysqli_num_rows($link, "SELECT 1 AS a", 1, 5);
	func_test_mysqli_num_rows($link, "SHOW VARIABLES LIKE '%nixnutz%'", 0, 10);
	func_test_mysqli_num_rows($link, "INSERT INTO test(id, label) VALUES (100, 'z')", NULL, 15);
	func_test_mysqli_num_rows($link, "SELECT id FROM test LIMIT 2", 2, 20, true);

	if ($res = mysqli_query($link, 'SELECT COUNT(id) AS num FROM test')) {

		$row = mysqli_fetch_assoc($res);
		mysqli_free_result($res);

		func_test_mysqli_num_rows($link, "SELECT id, label FROM test", (int)$row['num'], 25);

	} else {
		printf("[030] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	print "run_tests.php don't fool me with your 'ungreedy' expression '.+?'!\n";

	if ($res = mysqli_query($link, 'SELECT id FROM test', MYSQLI_USE_RESULT)) {

		$row = mysqli_fetch_row($res);
		if (0 !== ($tmp = mysqli_num_rows($res)))
			printf("[031] Expecting int/0, got %s/%d\n", gettype($tmp), $tmp);

		mysqli_free_result($res);

	} else {
		printf("[032] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_num_rows() expects parameter 1 to be mysqli_result, boolean given in %s on line %d

Warning: mysqli_free_result() expects parameter 1 to be mysqli_result, boolean given in %s on line %d

Warning: mysqli_num_rows(): Couldn't fetch mysqli_result in %s on line %d
run_tests.php don't fool me with your 'ungreedy' expression '.+?'!

Warning: mysqli_num_rows(): Function cannot be used with MYSQL_USE_RESULT in %s on line %d
done!