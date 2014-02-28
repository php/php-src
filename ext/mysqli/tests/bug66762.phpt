--TEST--
Bug #66762 	mysqli@libmysql segfault in mysqli_stmt::bind_result() when link closed
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$mysqli = new mysqli($host, $user, $passwd, $db);

	$read_stmt = $mysqli->prepare("SELECT 1");

	var_dump($read_stmt->bind_result($data));

	unset($mysqli);
	var_dump($read_stmt->bind_result($data));
	
?>
done!
--EXPECT--
bool(true)
bool(true)
done!