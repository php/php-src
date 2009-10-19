--TEST--
mysqli_fetch_field()
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
	if (!is_null($tmp = @$res->fetch_field()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');
	if (!$mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket))
		printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (!$res = $mysqli->query("SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1")) {
		printf("[003] [%d] %s\n", $mysqli->errno, $mysqli->error);
	}

	if (!is_null($tmp = @$res->fetch_field($link)))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	while ($tmp = $res->fetch_field()) {
		var_dump($tmp);
	}
	var_dump($tmp);

	$res->free_result();

	if (NULL !== ($tmp = $res->fetch_field()))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	$mysqli->close();
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
object(stdClass)#%d (11) {
  [%u|b%"name"]=>
  %unicode|string%(2) "ID"
  [%u|b%"orgname"]=>
  %unicode|string%(2) "id"
  [%u|b%"table"]=>
  %unicode|string%(4) "TEST"
  [%u|b%"orgtable"]=>
  %unicode|string%(4) "test"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(1)
  [%u|b%"length"]=>
  int(11)
  [%u|b%"charsetnr"]=>
  int(63)
  [%u|b%"flags"]=>
  int(49155)
  [%u|b%"type"]=>
  int(3)
  [%u|b%"decimals"]=>
  int(0)
}
object(stdClass)#%d (11) {
  [%u|b%"name"]=>
  %unicode|string%(5) "label"
  [%u|b%"orgname"]=>
  %unicode|string%(5) "label"
  [%u|b%"table"]=>
  %unicode|string%(4) "TEST"
  [%u|b%"orgtable"]=>
  %unicode|string%(4) "test"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(1)
  [%u|b%"length"]=>
  int(1)
  [%u|b%"charsetnr"]=>
  int(8)
  [%u|b%"flags"]=>
  int(0)
  [%u|b%"type"]=>
  int(254)
  [%u|b%"decimals"]=>
  int(0)
}
bool(false)

Warning: mysqli_result::fetch_field(): Couldn't fetch mysqli_result in %s on line %d
done!