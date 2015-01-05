--TEST--
mysqli_stmt_error()
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

	if (!is_null($tmp = @mysqli_stmt_error()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_error($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	// properly initialized?
	if ('' !== ($tmp = mysqli_stmt_error($stmt)))
		printf("[004] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

	if (mysqli_stmt_prepare($stmt, "SELECT i_do_not_exist_believe_me FROM test ORDER BY id"))
		printf("[005] Statement should have failed!\n");

	// set after error server?
	if ('' === ($tmp = mysqli_stmt_error($stmt)))
		printf("[006] Expecting string/any non empty, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, "SELECT id FROM test ORDER BY id"))
		printf("[007] [%d] %s\n", mysqli_stmt_error($stmt), mysqli_stmt_error($stmt));

	// reset after error & success
	if ('' !== ($tmp = mysqli_stmt_error($stmt)))
		printf("[008] Expecting empty string, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_kill($link, mysqli_thread_id($link));

	if (true === ($tmp = mysqli_stmt_execute($stmt)))
		printf("[009] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	// set after client error
	if ('' === ($tmp = mysqli_stmt_error($stmt)))
		printf("[010] Execting string/any non empty, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_stmt_close($stmt);

	if (NULL !== ($tmp = mysqli_stmt_error($stmt)))
		printf("[011] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_error(): Couldn't fetch mysqli_stmt in %s on line %d
done!