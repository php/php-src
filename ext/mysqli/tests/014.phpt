--TEST--
mysqli autocommit/commit/rollback
--SKIPIF--
<?php
	require_once('skipif.inc');
	require_once('skipifconnectfailure.inc');
	require_once("connect.inc");
	$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
	if (!$result = mysqli_query($link, "SHOW VARIABLES LIKE 'have_innodb'")) {
		die("skip Cannot check for required InnoDB suppot");
	}
	if (!$row = mysqli_fetch_row($result))
		die("skip Cannot check for required InnoDB suppot");

	mysqli_free_result($result);
	mysqli_close($link);
	if ($row[1] == "DISABLED" || $row[1] == "NO") {
		printf ("skip innodb support is not installed or enabled.");
		exit;
	}
?>
--FILE--
<?php
	require_once("connect.inc");
	$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	if (!mysqli_autocommit($link, TRUE))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "CREATE TABLE test(a int, b varchar(10)) engine=InnoDB"))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "INSERT INTO test VALUES (1, 'foobar')"))
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_autocommit($link, FALSE))
		printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "DELETE FROM test"))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "INSERT INTO test VALUES (2, 'egon')"))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_rollback($link))
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$result = mysqli_query($link, "SELECT * FROM test"))
		printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	printf("Num_of_rows=%d\n", mysqli_num_rows($result));
	if (!$row = mysqli_fetch_row($result))
		printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_free_result($result);

	var_dump($row);

	if (!mysqli_query($link, "DELETE FROM test"))
		printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "INSERT INTO test VALUES (2, 'egon')"))
		printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_commit($link))
		printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$result = mysqli_query($link, "SELECT * FROM test"))
		printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_row($result))
		printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

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
--EXPECTF--
Num_of_rows=1
array(2) {
  [0]=>
  %unicode|string%(1) "1"
  [1]=>
  %unicode|string%(6) "foobar"
}
array(2) {
  [0]=>
  %unicode|string%(1) "2"
  [1]=>
  %unicode|string%(4) "egon"
}
done!