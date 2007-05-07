--TEST--
Bug #35517 (mysqli_stmt_fetch returns NULL)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, "test");

	$mysql->query("CREATE TABLE temp (id INT UNSIGNED NOT NULL)");
	$mysql->query("INSERT INTO temp (id) VALUES (3000000897),(3800001532),(3900002281),(3100059612)");

	$stmt = $mysql->prepare("SELECT id FROM temp");
	$stmt->execute();
	$stmt->bind_result($id);
	while ($stmt->fetch()) {
		var_dump($id);
	}
	$stmt->close();

	$mysql->query("DROP TABLE temp");
	$mysql->close();
?>
--EXPECTF--
string(10) "3000000897"
string(10) "3800001532"
string(10) "3900002281"
string(10) "3100059612"
