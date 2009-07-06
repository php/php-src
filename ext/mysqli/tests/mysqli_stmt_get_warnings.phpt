--TEST--
mysqli_stmt_get_warnings() - TODO
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

	if (!is_null($tmp = @mysqli_stmt_get_warnings()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_get_warnings($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (NULL !== ($tmp = mysqli_stmt_get_warnings($stmt)))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, "DROP TABLE IF EXISTS test") || !mysqli_stmt_execute($stmt))
		printf("[005] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (false !== ($tmp = mysqli_stmt_get_warnings($stmt)))
		printf("[006] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, "CREATE TABLE test(id SMALLINT, label CHAR(1))") || !mysqli_stmt_execute($stmt))
		printf("[007] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (false !== ($tmp = mysqli_stmt_get_warnings($stmt)))
		printf("[008] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (100000, 'a'), (100001, 'b')") ||
		!mysqli_stmt_execute($stmt))
		printf("[009] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!is_object($warning = mysqli_stmt_get_warnings($stmt)))
		printf("[010] Expecting mysqli_warning object, got %s/%s\n", gettype($warning), $warning);

	if ('mysqli_warning' !== get_class($warning))
		printf("[011] Expecting object of type mysqli_warning got type '%s'", get_class($warning));

	if (!method_exists($warning, 'next'))
		printf("[012] Object mysqli_warning seems to lack method next()\n");

	$i = 0;
	do {

		if ('' == $warning->message)
			printf("[013 - %d] Message should not be empty\n", $i);

		if ('' == $warning->sqlstate)
			printf("[014 - %d] SQL State should not be empty\n", $i);

		if (0 == $warning->errno)
			printf("[015 - %d] Error number should not be zero\n", $i);

		$i++;

	} while ($warning->next());

	if (2 != $i)
		printf("[016] Expected 2 warnings, got %d warnings\n", $i);

	mysqli_stmt_close($stmt);

	if (NULL !== ($tmp = mysqli_stmt_get_warnings($stmt)))
		printf("[015] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_get_warnings(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_stmt_get_warnings(): Couldn't fetch mysqli_stmt in %s on line %d
done!