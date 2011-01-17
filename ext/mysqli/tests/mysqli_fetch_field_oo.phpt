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

	if (!is_null($tmp = @$res->fetch_field($link)))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	$charsets = my_get_charsets($link);

	if (!$res = $mysqli->query("SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1")) {
		printf("[004] [%d] %s\n", $mysqli->errno, $mysqli->error);
	}

	var_dump($res->fetch_field());

	$tmp = $res->fetch_field();
	var_dump($tmp);
	if ($tmp->charsetnr != $charsets['results']['nr']) {
		printf("[005] Expecting charset %s/%d got %d\n",
			$charsets['results']['charset'],
			$charsets['results']['nr'], $tmp->charsetnr);
	}
	if ($tmp->length != (1 * $charsets['results']['maxlen'])) {
		printf("[006] Expecting length %d got %d\n",
			$charsets['results']['maxlen'],
			$tmp->max_length);
	}
	if ($tmp->db != $db) {
		printf("008] Expecting database '%s' got '%s'\n",
		  $db, $tmp->db);
	}

	var_dump($res->fetch_field());

	$res->free_result();

	if (NULL !== ($tmp = $res->fetch_field()))
		printf("[007] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	$mysqli->close();
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
object(stdClass)#%d (13) {
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
  [%u|b%"db"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"catalog"]=>
  %unicode|string%(%d) "%s"
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
object(stdClass)#%d (13) {
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
  [%u|b%"db"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"catalog"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"max_length"]=>
  int(%d)
  [%u|b%"length"]=>
  int(%d)
  [%u|b%"charsetnr"]=>
  int(%d)
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