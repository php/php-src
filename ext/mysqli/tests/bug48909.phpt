--TEST--
Bug #48909 (Segmentation fault in mysqli_stmt_execute)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	if (!($link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)))
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (!$link->query("DROP TABLE IF EXISTS test") ||
		!$link->query(sprintf("CREATE TABLE test(id INT, label varchar(255)) ENGINE = %s", $engine)))
		printf("[002] [%d] %s\n", $link->errno, $link->error);

	if (!$stmt = $link->prepare("INSERT INTO test(id, label) VALUES  (?, ?)"))
		printf("[003] [%d] %s\n", $link->errno, $link->error);

	if (!$stmt->bind_param("bb",$bvar, $bvar))
		printf("[004] [%d] %s\n", $stmt->errno, $stmt->error);

	if (!$stmt->execute()) {
		if ($stmt->errno != 1366) {
			/*
				$bvar is null, b is for BLOB - any error like this should be OK:
				1366 -  Incorrect integer value: '' for column 'id' at row 1
			*/
			printf("[005] [%d] %s\n", $stmt->errno, $stmt->error);
		}
	}

	$stmt->close();
	$link->close();

	echo "done";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done