--TEST--
Multiple result set with PS
--SKIPIF--
<?php
require_once('skipif.inc');
require_once("connect.inc");
if (!$IS_MYSQLND) {
    die("skip mysqlnd only test");
}
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");
	require('table.inc');

	$stmt = mysqli_stmt_init($link);
	if (!$link->query('DROP PROCEDURE IF EXISTS p123')) {
		printf("[001] [%d] %s\n", $link->error, $link->errno);
	}

	if (!$link->query("CREATE PROCEDURE p123() BEGIN SELECT id+12, CONCAT_WS('-',label,'ahoi') FROM test ORDER BY id LIMIT 1; SELECT id + 42, CONCAT_WS('---',label, label) FROM test ORDER BY id LIMIT 1; END")) {
		printf("[002] [%d] %s\n", $link->error, $link->errno);
	}
	
	if (!($stmt = $link->prepare("CALL p123"))) {
		printf("[003] [%d] %s\n", $stmt->error, $stmt->errno);
	}

	if (!$stmt->execute()) {
		printf("[005] [%d] %s\n", $stmt->error, $stmt->errno);
	}

	$c_id = NULL;
	$c_label = NULL;
	if (!$stmt->bind_result($c_id, $c_label)) {
		printf("[004] [%d] %s\n", $stmt->error, $stmt->errno);
	}
	var_dump("pre:",$c_id, $c_label);

	if (!$stmt->fetch()) {
		printf("[006] [%d] %s\n", $stmt->error, $stmt->errno);
	}

	var_dump("post:",$c_id, $c_label);

	if ($stmt->fetch()) {
		printf("[007] Shouldn't have fetched anything\n");
		var_dump($c_id, $c_label);
	}
	if (!$stmt->more_results()) {
		printf("[008] Expected more results\n");
	} else {
		var_dump("next_result:", $stmt->next_result());
	}

	if (!$stmt->bind_result($c_id, $c_label)) {
		printf("[004] [%d] %s\n", $stmt->error, $stmt->errno);
	}
	var_dump("pre:",$c_id, $c_label);

	if (!$stmt->fetch()) {
		printf("[009] [%d] %s\n", $stmt->error, $stmt->errno);
	}

	var_dump("post:",$c_id, $c_label);

	if (!$stmt->more_results()) {
		printf("[010] Expected more results\n");
	} else {
		var_dump("next_result:", $stmt->next_result());
	}

	if (!$stmt->more_results()) {
		printf("[010] Expected more results\n");
	} else {
		var_dump("next_result:", $stmt->next_result());
	}

	if ($stmt->more_results()) {
		printf("[011] No more results expected\n");
	}

	$stmt->close();
	$link->close();


	echo "done";
?>
--CLEAN--
<?php
	require_once("connect.inc");
	if (!$link->query('DROP PROCEDURE IF EXISTS p123')) {
		printf("[001] [%d] %s\n", $link->error, $link->errno);
	}
?>
--EXPECTF--
done
