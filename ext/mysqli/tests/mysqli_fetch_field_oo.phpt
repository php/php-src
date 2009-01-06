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
	include "connect.inc";

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

	while ($tmp = $res->fetch_field())
		var_dump($tmp);
	var_dump($tmp);

	$res->free_result();

	if (NULL !== ($tmp = $res->fetch_field()))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	$mysqli->close();
	print "done!";
?>
--EXPECTF--
object(stdClass)#%d (11) {
  [u"name"]=>
  unicode(2) "ID"
  [u"orgname"]=>
  unicode(2) "id"
  [u"table"]=>
  unicode(4) "TEST"
  [u"orgtable"]=>
  unicode(4) "test"
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(%d)
  [u"length"]=>
  int(%d)
  [u"charsetnr"]=>
  int(%d)
  [u"flags"]=>
  int(%d)
  [u"type"]=>
  int(%d)
  [u"decimals"]=>
  int(0)
}
object(stdClass)#%d (11) {
  [u"name"]=>
  unicode(5) "label"
  [u"orgname"]=>
  unicode(5) "label"
  [u"table"]=>
  unicode(4) "TEST"
  [u"orgtable"]=>
  unicode(4) "test"
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(%d)
  [u"length"]=>
  int(%d)
  [u"charsetnr"]=>
  int(%d)
  [u"flags"]=>
  int(%d)
  [u"type"]=>
  int(%d)
  [u"decimals"]=>
  int(0)
}
bool(false)

Warning: mysqli_result::fetch_field(): Couldn't fetch mysqli_result in %s on line %d
done!
