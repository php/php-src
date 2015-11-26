--TEST--
Bug #35759 (mysqli_stmt_bind_result() makes huge allocation when column empty)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

	require_once("connect.inc");
	$col_num= 1000;

	$mysql = new mysqli($host, $user, $passwd, $db, $port, $socket);
	$mysql->query("DROP TABLE IF EXISTS test");
	$create = "CREATE TABLE test (a0 MEDIUMBLOB NOT NULL DEFAULT ''";
	$i= 0;
	while (++$i < $col_num) {
		$create .= ", a$i MEDIUMBLOB NOT NULL DEFAULT ''";
	}
	$create .= ") ENGINE=MyISAM"; // doesn't work with InnoDB, which is default in 5.5

	if (!$mysql->query($create)) {
		if (1101 == $mysql->errno) {
			/* SQL strict mode - [1101] BLOB/TEXT column 'a0' can't have a default value */
			print "done!";
			exit(0);
		}
		printf("[001] [%d] %s\n", $mysql->errno, $mysql->error);
	}

	if (!$mysql->query("INSERT INTO test (a0) VALUES ('')"))
		printf("[002] [%d] %s\n", $mysql->errno, $mysql->error);

	$stmt = $mysql->prepare("SELECT * FROM test");
	if ($stmt) {

		$stmt->execute();
		$stmt->store_result();
		for ($i = 0; $i < $col_num; $i++) {
			$params[] = &$col_num;
		}
		call_user_func_array(array($stmt, "bind_result"), $params);
		$stmt->fetch();

		$stmt->close();
	} else {
		printf("[003] [%d] %s\n", $mysql->errno, $mysql->error);
	}

	$mysql->close();

	echo "done!";
?>
--CLEAN--
<?php require("clean_table.inc"); ?>
--EXPECT--
done!