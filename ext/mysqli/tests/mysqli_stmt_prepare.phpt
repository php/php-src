--TEST--
mysqli_stmt_prepare()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	// Note: No SQL tests here! We can expand one of the *fetch()
	// tests to a generic SQL test, if we ever need that.
	// We would duplicate the SQL test cases if we have it here and in one of the
	// fetch tests, because the fetch tests would have to call prepare/execute etc.
	// anyway.

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_stmt_prepare()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_prepare($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (NULL !== ($tmp = @mysqli_stmt_prepare($stmt)))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (false !== ($tmp = mysqli_stmt_prepare($stmt, '')))
		printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = mysqli_stmt_prepare($stmt, 'SELECT id FROM test')))
		printf("[006] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_stmt_close($stmt);

	if (NULL !== ($tmp = mysqli_stmt_prepare($stmt, "SELECT id FROM test")))
		printf("[007] Expecting NULL, got %s/%s\n");

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_prepare(): Couldn't fetch mysqli_stmt in %s on line %d
done!
