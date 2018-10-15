--TEST--
mysqli_stmt_bind_param() - binding variable twice
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require('table.inc');

	function bind_twice($link, $engine, $sql_type1, $sql_type2, $bind_type1, $bind_type2, $bind_value1, $bind_value2, $offset) {

		if (!mysqli_query($link, "DROP TABLE IF EXISTS test")) {
			printf("[%03d + 1] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
			return false;
		}
		mysqli_autocommit($link, true);

		$sql = sprintf("CREATE TABLE test(col1 %s, col2 %s) ENGINE=%s", $sql_type1, $sql_type2, $engine);
		if (!mysqli_query($link, $sql)) {
			printf("[%03d + 2] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		if (!$stmt = mysqli_stmt_init($link)) {
			printf("[%03d + 3] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(col1, col2) VALUES (?, ?)")) {
			printf("[%03d + 4] [%d] %s\n", $offset, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		if (!mysqli_stmt_bind_param($stmt, $bind_type1 . $bind_type2, $bind_value1, $bind_value1)) {
			printf("[%03d + 5] [%d] %s\n", $offset, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		if (!mysqli_stmt_execute($stmt)) {
			printf("[%03d + 6] [%d] %s\n", $offset, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		if (!mysqli_stmt_bind_param($stmt, $bind_type1 . $bind_type2, $bind_value1, $bind_value2)) {
			printf("[%03d + 7] [%d] %s\n", $offset, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}
		if (!mysqli_stmt_execute($stmt)) {
			printf("[%03d + 8] [%d] %s\n", $offset, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		mysqli_stmt_close($stmt);
		if (!$res = mysqli_query($link, "SELECT col1, col2 FROM test")) {
			printf("[%03d + 9] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		if (2 !== ($tmp = mysqli_num_rows($res))) {
			printf("[%03d + 10] Expecting 2 rows, got %d rows [%d] %s\n", $offset, $tmp, mysqli_errno($link), mysqli_error($link));
		}

		$row = mysqli_fetch_assoc($res);
		if (($row['col1'] != $bind_value1) || ($row['col2'] != $bind_value1)) {
			printf("[%03d + 11] Expecting col1 = %s, col2 = %s got col1 = %s, col2 = %s - [%d] %s\n",
				$offset, $bind_value1, $bind_value1,
				$row['col1'], $row['col2'],
				mysqli_errno($link), mysqli_error($link));
			return false;
		}

		$row = mysqli_fetch_assoc($res);
		if (($row['col1'] != $bind_value1) || ($row['col2'] != $bind_value2)) {
			printf("[%03d + 12] Expecting col1 = %s, col2 = %s got col1 = %s, col2 = %s - [%d] %s\n",
				$offset, $bind_value1, $bind_value2,
				$row['col1'], $row['col2'],
				mysqli_errno($link), mysqli_error($link));
			return false;
		}
		mysqli_free_result($res);
		return true;
	}

	bind_twice($link, $engine, 'CHAR(1)', 'CHAR(1)', 's', 's', 'a', 'b', 10);
	bind_twice($link, $engine, 'INT', 'INT', 'i', 'i', 1, 2, 20);
	bind_twice($link, $engine, 'FLOAT', 'FLOAT', 'd', 'd', 1.01, 1.02, 30);

	/* type juggling - note that int->char works */
	bind_twice($link, $engine, 'CHAR(1)', 'CHAR(1)', 's', 's', 1, 2, 40);
	/* type juggling - note that string->integer works */
	bind_twice($link, $engine, 'INT', 'INT', 'i', 'i', '1', '2', 50);
	/* type juggling - note that string->float works*/
	bind_twice($link, $engine, 'FLOAT', 'FLOAT', 'd', 'd', '1.01', '1.02', 60);

	/* now, let's have two columns of different type and do type juggling */
	/*
	what the test will do is:
		1) col1 INT, col2 CHAR(1)
    2) bind_param('is', 1, 1)
    3) execute()
		4) bind_param('is', 1, 'a')
		5) execute()

		col1 INT, col2 INT
    bind_param('ii', '1', '2')	--> OK  (int column, string value)
		bind_param('ii', 1, 2) 			--> OK  (int column, int value)
    col1 CHAR(1), col2 CHAR(2)
    bind_param('ss', 1, 2)  		--> OK (string column, int value)

		So, what about:
		col1 INT, COL2 CHAR(1)
		bind_param('is', 1, 1)     ---> ??
	*/
	bind_twice($link, $engine, 'INT', 'CHAR(1)', 'i', 's', 1, 'a', 70);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECT--
done!
