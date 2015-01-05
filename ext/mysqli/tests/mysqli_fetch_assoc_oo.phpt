--TEST--
mysqli_fetch_assoc()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	// Note: no SQL type tests, internally the same function gets used as for mysqli_fetch_array() which does a lot of SQL type test
	$mysqli = new mysqli();
	$res = @new mysqli_result($mysqli);
	if (!is_null($tmp = @$res->fetch_assoc()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');
	if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
		printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (!is_null($tmp = @$res->fetch_assoc($link)))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$res = $mysqli->query("SELECT id, label FROM test ORDER BY id LIMIT 1")) {
		printf("[004] [%d] %s\n", $mysqli->errno, $mysqli->error);
	}

	print "[005]\n";
	var_dump($res->fetch_assoc());

	print "[006]\n";
	var_dump($res->fetch_assoc());

	$res->free_result();

	if (!$res = $mysqli->query("SELECT 1 AS a, 2 AS a, 3 AS c, 4 AS C, NULL AS d, true AS e")) {
		printf("[007] Cannot run query, [%d] %s\n", $mysqli->errno, $mysqli->error);
	}
	print "[008]\n";
	var_dump($res->fetch_assoc());

	$res->free_result();

	if (NULL !== ($tmp = $res->fetch_assoc()))
		printf("[008] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
[005]
array(2) {
  [%u|b%"id"]=>
  %unicode|string%(1) "1"
  [%u|b%"label"]=>
  %unicode|string%(1) "a"
}
[006]
NULL
[008]
array(5) {
  [%u|b%"a"]=>
  %unicode|string%(1) "2"
  [%u|b%"c"]=>
  %unicode|string%(1) "3"
  [%u|b%"C"]=>
  %unicode|string%(1) "4"
  [%u|b%"d"]=>
  NULL
  [%u|b%"e"]=>
  %unicode|string%(1) "1"
}

Warning: mysqli_result::fetch_assoc(): Couldn't fetch mysqli_result in %s on line %d
done!