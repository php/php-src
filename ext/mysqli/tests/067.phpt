--TEST--
function test: nested selects (cursors)
--SKIPIF--
<?php 
	require_once('skipif.inc'); 
	/* skip cursor test for versions < 50004 */
	if (mysqli_get_client_version() < 50009) {
		die("skip Client library doesn't support cursors");	
	}
?>
--FILE--
<?php

	function open_cursor($mysql, $query) {
		$stmt = $mysql->prepare($query);
		$stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_READ_ONLY);
		return $stmt;
	}

	include "connect.inc";
	$a = array();
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$mysql = new mysqli($host, $user, $passwd, "test");

	for ($i=0;$i < 3; $i++) {
		$mysql->query("DROP TABLE IF EXISTS cursor$i");
		$mysql->query("CREATE TABLE cursor$i (a int not null)");
		$mysql->query("INSERT INTO cursor$i VALUES (1),(2),(3),(4),(5),(6)");
		$stmt[$i] = open_cursor($mysql, "SELECT a FROM cursor$i");
		$stmt[$i]->execute();
		$stmt[$i]->bind_result($a[$i]);
	}

	
	$cnt = 0;
	while ($stmt[0]->fetch()) {
		$stmt[1]->fetch();
		$stmt[2]->fetch();
		$cnt += $a[0] + $a[1] + $a[2];
	}

	for ($i=0; $i < 3; $i++) {
		$stmt[$i]->close();
	}

	$mysql->close();
	var_dump($cnt);
?>
--EXPECT--
int(63)
