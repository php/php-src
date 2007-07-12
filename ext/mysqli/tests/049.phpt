--TEST--
mysql_fetch_row (OO-Style)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	/*** test mysqli_connect 127.0.0.1 ***/
	$mysql = mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	$mysql->select_db($db);
	$result = $mysql->query("SELECT DATABASE()");
	$row = $result->fetch_row();
	$result->close();

	var_dump($row);

	$mysql->close();
	print "done!";
?>
--EXPECTF--
array(1) {
  [0]=>
  string(%d) "%s"
}
done!
--UEXPECTF--
array(1) {
  [0]=>
  unicode(%d) "%s"
}
done!