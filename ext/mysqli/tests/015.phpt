--TEST--
mysqli autocommit/commit/rollback with innodb
--SKIPIF--
<?php
	require_once('skipif.inc');
	include "connect.inc";
	$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);
	$result = mysqli_query($link, "SHOW VARIABLES LIKE 'have_innodb'");
	$row = mysqli_fetch_row($result);
	mysqli_free_result($result);
	mysqli_close($link);

	if ($row[1] == "NO") {
		printf ("skip innodb support not installed.");
	}
?>
--FILE--
<?php
	include "connect.inc";

	$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	mysqli_select_db($link, $db);

	mysqli_autocommit($link, TRUE);

  	mysqli_query($link,"DROP TABLE IF EXISTS ac_01");

	mysqli_query($link,"CREATE TABLE ac_01(a int, b varchar(10)) Engine=InnoDB");

	mysqli_query($link, "INSERT INTO ac_01 VALUES (1, 'foobar')");
	mysqli_autocommit($link, FALSE);

	mysqli_query($link, "DELETE FROM ac_01");
	mysqli_query($link, "INSERT INTO ac_01 VALUES (2, 'egon')");

	mysqli_rollback($link);

	$result = mysqli_query($link, "SELECT SQL_NO_CACHE * FROM ac_01");
	$row = mysqli_fetch_row($result);
	mysqli_free_result($result);

	var_dump($row);

	mysqli_query($link, "DELETE FROM ac_01");
	mysqli_query($link, "INSERT INTO ac_01 VALUES (2, 'egon')");
	mysqli_commit($link);

	$result = mysqli_query($link, "SELECT * FROM ac_01");
	$row = mysqli_fetch_row($result);
	mysqli_free_result($result);

	var_dump($row);

	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(6) "foobar"
}
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(4) "egon"
}
done!
--UEXPECTF--
array(2) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(6) "foobar"
}
array(2) {
  [0]=>
  unicode(1) "2"
  [1]=>
  unicode(4) "egon"
}
done!