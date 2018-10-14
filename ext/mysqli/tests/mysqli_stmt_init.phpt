--TEST--
mysqli_stmt_init()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	/*
	NOTE: no datatype tests here! This is done by
	mysqli_stmt_bind_result.phpt already. Restrict
	this test case to the basics.
	*/
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_stmt_init()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_init($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!is_object($stmt = mysqli_stmt_init($link)))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!is_object($stmt2 = @mysqli_stmt_init($link)))
		printf("[003a] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_stmt_close($stmt);

	if (NULL !== ($tmp = mysqli_stmt_init($stmt)))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	if (NULL !== ($tmp = mysqli_stmt_init($link)))
		printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_close(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_stmt_init() expects parameter 1 to be mysqli, object given in %s on line %d

Warning: mysqli_stmt_init(): Couldn't fetch mysqli in %s on line %d
done!
