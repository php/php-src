--TEST--
mysqli_float_handling - ensure 4 byte float is handled correctly
--SKIPIF--
<?php
	require_once('skipif.inc');
	require_once('skipifemb.inc');
	require_once('skipifconnectfailure.inc');
?>
--INI--
precision=5
--FILE--
<?php
	require('connect.inc');
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		die();
	}


	if (!mysqli_query($link, "DROP TABLE IF EXISTS test")) {
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		die();
	}

	if (!mysqli_query($link, "CREATE TABLE test(id INT PRIMARY KEY, fp4 FLOAT, fp8 DOUBLE) ENGINE = InnoDB")) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		die();
	}

	// Insert via string to make sure the real floating number gets to the DB
	if (!mysqli_query($link, "INSERT INTO test(id, fp4, fp8) VALUES (1, 9.9999, 9.9999)")) {
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		die();
	}

	if (!($stmt = mysqli_prepare($link, "SELECT id, fp4, fp8 FROM test"))) {
		printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		die();
	}

	$id = null;
	$fp4 = null;
	$fp8 = null;

	if (!mysqli_stmt_bind_result($stmt, $id, $fp4, $fp8)) {
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		die();
	}

	if (!mysqli_stmt_execute($stmt)) {
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		die();
	}


	if (!(mysqli_stmt_fetch($stmt))) {
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		die();
	}

	print $id . ": " . $fp4 . ": " . $fp8 . "\n";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
1: 9.9999: 9.9999
