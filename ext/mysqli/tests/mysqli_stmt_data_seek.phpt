--TEST--
mysqli_stmt_data_seek()
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

	if (!is_null($tmp = @mysqli_stmt_data_seek()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_data_seek($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (false !== ($tmp = mysqli_stmt_data_seek($stmt, 1)))
		printf("[004] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, "SELECT id FROM test ORDER BY id"))
		printf("[005] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (true !== ($tmp = mysqli_stmt_execute($stmt)))
		printf("[006] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);


	$id = null;
	if (!mysqli_stmt_bind_result($stmt, $id))
		printf("[007] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_store_result($stmt))
		printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!is_null($tmp = mysqli_stmt_data_seek($stmt, 2)))
		printf("[009] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_fetch($stmt))
		printf("[010] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	var_dump($id);

	if (!is_null($tmp = mysqli_stmt_data_seek($stmt, 0)))
		printf("[011] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_fetch($stmt))
		printf("[012] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	var_dump($id);

	if (!is_null($tmp = mysqli_stmt_data_seek($stmt, mysqli_stmt_num_rows($stmt) + 100)))
		printf("[013] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (mysqli_stmt_fetch($stmt))
		printf("[014] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	var_dump($id);

	if (false !== ($tmp = mysqli_stmt_data_seek($stmt, -1)))
		printf("[015] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (mysqli_stmt_fetch($stmt))
		printf("[016] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	var_dump($id);

	mysqli_stmt_close($stmt);

	if (false !== ($tmp = mysqli_stmt_data_seek($stmt, 0)))
		printf("[017] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_data_seek(): invalid object or resource mysqli_stmt
 in %s on line %d
int(3)
int(1)
int(1)

Warning: mysqli_stmt_data_seek(): Offset must be positive in %s on line %d
int(1)

Warning: mysqli_stmt_data_seek(): Couldn't fetch mysqli_stmt in %s on line %d
done!
