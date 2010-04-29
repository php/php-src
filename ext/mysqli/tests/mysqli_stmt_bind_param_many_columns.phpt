--TEST--
mysqli_stmt_bind_param() - Binding with very high number of columns
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
memory_limit=256M
--FILE--
<?php
	/*
	The way we test the INSERT and data types overlaps with
	the mysqli_stmt_bind_result test in large parts. There is only
	one difference. This test uses mysqli_query()/mysqli_fetch_assoc() to
	fetch the inserted values. This way we test
	mysqli_query()/mysqli_fetch_assoc() for all possible data types
	in this file and we test mysqli_stmt_bind_result() in the other
	test -- therefore the "duplicate" makes some sense to me.
	*/
	require_once("connect.inc");

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
		exit(1);
	}

	list($old_max_allowed_packet) = $link->query("SELECT @@max_allowed_packet")->fetch_row();
	if (!$link->query("SET GLOBAL max_allowed_packet=(2<<29)")) {
		printf("Failed to set max_allowed_packet the test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	mysqli_close($link);


	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
		exit(1);
	}

	if (!mysqli_query($link, 'DROP TABLE IF EXISTS ps_test')) {
		printf("Failed to drop old test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		exit(1);
	}

	$cols = 2500;
	$str = array();
	for ($i = 1; $i <= $cols; $i++) {
		$str[] ="a$i BLOB";
	}
	$link->query("CREATE TABLE ps_test(" . implode(" , ", $str) . ") ENGINE=MyISAM");
	if (mysqli_errno($link)) {
		printf("Failed to create the test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		die("");
	}
	$stmt = $link->prepare("INSERT INTO ps_test VALUES(".str_repeat("?, ", $cols-1) . "?)");
	var_dump($stmt->id);
	$s = str_repeat("a", 2 << 12);
	$eval_str="\$stmt->bind_param(\"".str_repeat("s",$cols)."\", ";
	for ($i = 1; $i < $cols; $i++) {
		$eval_str.="\$s,";
	}
	$eval_str.="\$s";
	$eval_str.=");";
	eval($eval_str);
	printf("executing\n");
	if (!$stmt->execute()) {
		printf("failed");
		printf("Failed to execute: [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	} else {
		var_dump(true);
	}

	mysqli_stmt_close($stmt);


	if (!$link->query("SET GLOBAL max_allowed_packet=$old_max_allowed_packet")) {
		printf("Failed to set max_allowed_packet the test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	mysqli_close($link);

	print "done!";
?>
--CLEAN--
<?php
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
		exit(1);
	}
	if (!mysqli_query($link, 'DROP TABLE IF EXISTS ps_test')) {
		printf("Failed to drop the test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		exit(1);
	}
?>
--EXPECTF--
int(1)
executing
bool(true)
done!
