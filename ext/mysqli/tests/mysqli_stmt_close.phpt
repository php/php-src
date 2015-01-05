--TEST--
mysqli_stmt_close()
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

	if (!is_null($tmp = @mysqli_stmt_close()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_close($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	// Yes, amazing, eh? AFAIK a work around of a constructor bug...
	if (!is_null($tmp = mysqli_stmt_close($stmt)))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test"))
		printf("[005] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (true !== ($tmp = mysqli_stmt_close($stmt)))
		printf("[006] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = mysqli_stmt_close($stmt)))
		printf("[007] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$stmt = mysqli_stmt_init($link))
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (?, ?)"))
		printf("[009] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_param($stmt, "is", $id, $label))
		printf("[010] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = 100; $label = 'z';
	if (!mysqli_stmt_execute($stmt))
		printf("[011] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	mysqli_kill($link, mysqli_thread_id($link));

	if (true !== ($tmp = mysqli_stmt_close($stmt)))
		printf("[012] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	require('table.inc');
	if (!$stmt = mysqli_stmt_init($link))
		printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test"))
		printf("[014] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label))
		printf("[015] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt) || !mysqli_stmt_fetch($stmt))
		printf("[016] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	mysqli_kill($link, mysqli_thread_id($link));

	if (true !== ($tmp = mysqli_stmt_close($stmt)))
		printf("[017] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_close(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_stmt_close(): Couldn't fetch mysqli_stmt in %s on line %d
done!