--TEST--
Fetching BIT column values using the PS API
--SKIPIF--
<?php
	require_once('skipif.inc');
	require_once('skipifemb.inc');
	require_once('skipifconnectfailure.inc');
	require_once('connect.inc');
	require_once('table.inc');
	if (mysqli_get_server_version($link) < 50003)
		// b'001' syntax not supported before 5.0.3
		die("skip Syntax used for test not supported with MySQL Server before 5.0.3");
	if (!$IS_MYSQLND && (mysqli_get_client_version() < 50003))
		// better don't trust libmysql before 5.0.3
		die("skip Syntax used for test not supported with MySQL Server before 5.0.3");
?>
--FILE--
<?php
	require('connect.inc');

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	/* NOTE: works only for up to 31 bits! This limitation should be documented. */
	for ($bits = 1; $bits < 32; $bits++) {
		$max_value = pow(2, $bits) - 1;
		$tests = 0;
		if (!mysqli_query($link, "DROP TABLE IF EXISTS test") ||
			!mysqli_query($link, $sql = sprintf('CREATE TABLE test(id INT, label BIT(%d)) ENGINE="%s"', $bits, $engine)))
			printf("[002 - %d] [%d] %s\n",$bits, mysqli_errno($link), mysqli_error($link));

		if (!$stmt = mysqli_stmt_init($link))
			printf("[003 - %d] [%d] %s\n", $bits, mysqli_errno($link), mysqli_error($link));

		while ($tests < min($max_value, 20)) {
			$tests++;
			$value = mt_rand(0, $max_value);
			$sql = sprintf("INSERT INTO test(id, label) VALUES (%d, b'%s')", $value, decbin($value));

			if (!mysqli_stmt_prepare($stmt, $sql) ||
					!mysqli_stmt_execute($stmt))
				printf("[004 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

			$id = $_label0 = $label = null;
			$sql = sprintf("SELECT id, label + 0 AS _label0, label FROM test WHERE id = %d", $value);
			if (!mysqli_stmt_prepare($stmt, $sql) ||
					!mysqli_stmt_execute($stmt) ||
					!mysqli_stmt_bind_result($stmt, $id, $_label0, $label))
				printf("[005 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

			if (!mysqli_stmt_fetch($stmt))
				printf("[006 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

			if (($id !== $_label0) || ($value !== $_label0)) {
				printf("[007 - %d] Insert of %d in BIT(%d) column might have failed. MySQL reports odd values, id = %s, _label0 = %s, label = %s.\n", $bits, $value, $bits, $id, $_label0, $label);
			}
			if ($value != $label) {
				printf("[008 - %d] Wrong values, (original) value = %s, id = %s,  label + 0 AS label0 = %s, label = %s\n",
					$bits, $value, $id, $_label0, $label);
			}
		}

		mysqli_stmt_close($stmt);

	}

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done!
