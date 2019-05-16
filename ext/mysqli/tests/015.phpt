--TEST--
mysqli autocommit/commit/rollback with innodb
--SKIPIF--
<?php
	require_once('skipif.inc');
	require_once('skipifconnectfailure.inc');

	require_once('connect.inc');
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		die(sprintf("Cannot connect, [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

	if (!have_innodb($link))
		die(sprintf("Needs InnoDB support, [%d] %s", $link->errno, $link->error));
?>
--FILE--
<?php
	require_once("connect.inc");

	$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
	if (!$link)
		printf("[001] Cannot connect, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	if (!mysqli_select_db($link, $db))
		printf("[002] Cannot select DB '%s', [%d] %s\n", $db,
			mysqli_errno($link), mysqli_error($link));

	if (!mysqli_autocommit($link, TRUE))
		printf("[003] Cannot turn on autocommit mode, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link,"DROP TABLE IF EXISTS test") ||
		!mysqli_query($link,"CREATE TABLE test(a int, b varchar(10)) Engine=InnoDB") ||
		!mysqli_query($link, "INSERT INTO test VALUES (1, 'foobar')"))
		printf("[004] Cannot create test data, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	if (!mysqli_autocommit($link, FALSE))
		printf("[005] Cannot turn off autocommit mode, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "DELETE FROM test") ||
			!mysqli_query($link, "INSERT INTO test VALUES (2, 'egon')"))
		printf("[006] Cannot modify test data, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	if (!mysqli_rollback($link))
		printf("[007] Cannot call rollback, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	$result = mysqli_query($link, "SELECT SQL_NO_CACHE * FROM test");
	if (!$result)
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$row = mysqli_fetch_row($result);
	mysqli_free_result($result);

	var_dump($row);

	if (!mysqli_query($link, "DELETE FROM test") ||
			!mysqli_query($link, "INSERT INTO test VALUES (2, 'egon')"))
		printf("[009] Cannot modify test data, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	mysqli_commit($link);

	$result = mysqli_query($link, "SELECT * FROM test");
	if (!$result)
		printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$row = mysqli_fetch_row($result);
	mysqli_free_result($result);

	var_dump($row);

	mysqli_query($link, "DROP TABLE IF EXISTS test");
	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(6) "foobar"
}
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(4) "egon"
}
done!
