--TEST--
mysqli_stmt_param_count()
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

	if (!is_null($tmp = @mysqli_stmt_param_count()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_param_count($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (false !== ($tmp = mysqli_stmt_param_count($stmt)))
		printf("[004] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	function func_test_mysqli_stmt_param_count($stmt, $query, $expected, $offset) {

		if (!mysqli_stmt_prepare($stmt, $query)) {
			printf("[%03d] [%d] %s\n", $offset, mysqli_stmt_errno($stmt), mysqli_error($stmt));
			return false;
		}

		if ($expected !== ($tmp = mysqli_stmt_param_count($stmt)))
			printf("[%03d] Expecting %s/%d, got %s/%d\n", $offset + 3,
				gettype($expected), $expected,
				gettype($tmp), $tmp);
		return true;
	}

	func_test_mysqli_stmt_param_count($stmt, "SELECT 1 AS a", 0, 10);
	func_test_mysqli_stmt_param_count($stmt, "INSERT INTO test(id) VALUES (?)", 1, 20);
	func_test_mysqli_stmt_param_count($stmt, "INSERT INTO test(id, label) VALUES (?, ?)", 2, 30);
	func_test_mysqli_stmt_param_count($stmt, "INSERT INTO test(id, label) VALUES (?, '?')", 1, 40);

	mysqli_stmt_close($stmt);

	if (false !== ($tmp = mysqli_stmt_param_count($stmt)))
		printf("[40] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	/* Check that the function alias exists. It's a deprecated function,
	but we have not announce the removal so far, therefore we need to check for it */
	if (!is_null($tmp = @mysqli_stmt_param_count()))
		printf("[041] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_param_count(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_stmt_param_count(): Couldn't fetch mysqli_stmt in %s on line %d
done!
