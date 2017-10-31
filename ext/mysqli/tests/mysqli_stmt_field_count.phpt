--TEST--
mysqli_stmt_field_counts()
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

	if (!is_null($tmp = @mysqli_stmt_field_count()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_field_count($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	$stmt = mysqli_stmt_init($link);
	if (false !== ($tmp = mysqli_stmt_field_count($stmt)))
		printf("[003] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	if (mysqli_stmt_prepare($stmt, ''))
		printf("[004] Prepare should fail for an empty statement\n");

	if (false !== ($tmp = mysqli_stmt_field_count($stmt)))
		printf("[005] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, 'SELECT 1'))
		printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	if (1 !== ($tmp = mysqli_stmt_field_count($stmt)))
		printf("[007] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, 'SELECT 1, 2'))
		printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	if (2 !== ($tmp = mysqli_stmt_field_count($stmt)))
		printf("[009] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test'))
		printf("[010] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	if (2 !== ($tmp = mysqli_stmt_field_count($stmt)))
		printf("[011] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, 'SELECT label FROM test') ||
		!mysqli_stmt_execute($stmt))
		printf("[012] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	if (1 !== ($tmp = mysqli_stmt_field_count($stmt)))
		printf("[013] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

	$label = null;
	if (mysqli_stmt_bind_param($stmt, "s", $label))
		printf("[014] expected error - got ok\n");
	while (mysqli_stmt_fetch($stmt))
		if (1 !== ($tmp = mysqli_stmt_field_count($stmt)))
			printf("[015] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, 'INSERT INTO test(id) VALUES (100)'))
		printf("[016] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	if (0 !== ($tmp = mysqli_stmt_field_count($stmt)))
		printf("[017] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, "UPDATE test SET label = 'z' WHERE id = 1") ||
		!mysqli_stmt_execute($stmt))
		printf("[018] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (0 !== ($tmp = mysqli_stmt_field_count($stmt)))
		printf("[019] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_stmt_close($stmt);

	if (mysqli_stmt_prepare($stmt, 'SELECT id FROM test'))
		printf("[020] Prepare should fail, statement has been closed\n");

	if (false !== ($tmp = mysqli_stmt_field_count($stmt)))
		printf("[021] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_field_count(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_stmt_field_count(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_stmt_bind_param(): Number of variables doesn't match number of parameters in prepared statement in %s on line %d

Warning: mysqli_stmt_prepare(): Couldn't fetch mysqli_stmt in %s on line %d

Warning: mysqli_stmt_field_count(): Couldn't fetch mysqli_stmt in %s on line %d
done!
