--TEST--
mysqli_stmt_reset()
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

	if (!is_null($tmp = @mysqli_stmt_reset()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_reset($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (NULL !== ($tmp = mysqli_stmt_reset($stmt)))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = mysqli_stmt_prepare($stmt, 'SELECT id FROM test')))
		printf("[005] Expecting boolean/true, got %s/%s, [%d] %s\n",
			gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (true !== ($tmp = mysqli_stmt_reset($stmt)))
		printf("[006] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = mysqli_stmt_execute($stmt)))
		printf("[007] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	$id = null;
	if (!mysqli_stmt_bind_result($stmt, $id))
		printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_fetch($stmt))
		printf("[009] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	var_dump($id);
	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
		printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT, label BLOB, PRIMARY KEY(id))"))
		printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(label) VALUES (?)"))
		printf("[013] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$label = null;
	if (!mysqli_stmt_bind_param($stmt, "b", $label))
		printf("[014] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$label = 'abc';
	for ($i = 0; $i < 10; $i++) {
		if (!mysqli_stmt_send_long_data($stmt, 0, $label))
			printf("[015 - %d] [%d] %s\n", $i, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	if (!mysqli_stmt_reset($stmt))
		printf("[016] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[017] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!$res = mysqli_query($link, "SELECT label FROM test"))
		printf("[018] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[019] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_free_result($res);

	if ($row['label'] != '')
		printf("[020] Expecting empty string, got string/%s\n", $row['label']);

	mysqli_stmt_close($stmt);

	if (NULL !== ($tmp = mysqli_stmt_reset($stmt)))
		printf("[021] Expecting NULL, got %s/%s\n");

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_reset(): invalid object or resource mysqli_stmt
 in %s on line %d
int(1)

Warning: mysqli_stmt_reset(): Couldn't fetch mysqli_stmt in %s on line %d
done!
