--TEST--
mysql_fetch_object
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'connect.inc';

class class24 {
	function __construct() {
		echo __METHOD__ . "\n";
	}
}

$data = array(
	"one",
	"two",
	"three"
	);

$db = mysql_connect($host, $user, $passwd);

mysql_select_db("test");

mysql_query('DROP TABLE IF EXISTS test');

mysql_query("CREATE TABLE test(a varchar(10))");

foreach ($data as $str) {
	mysql_query("INSERT INTO test VALUES('$str')");
	var_dump($str);
}

echo "==stdClass==\n";
$res = mysql_query("SELECT a FROM test");
while ($obj = mysql_fetch_object($res)) {
	var_dump($obj);
}

echo "==class24==\n";
$res = mysql_query("SELECT a FROM test");
while ($obj = mysql_fetch_object($res, 'class24')) {
	var_dump($obj);
}

mysql_close($db);

?>
==DONE==
--EXPECTF--
string(3) "one"
string(3) "two"
string(5) "three"
==stdClass==
object(stdClass)#%d (1) {
  ["a"]=>
  string(3) "one"
}
object(stdClass)#%d (1) {
  ["a"]=>
  string(3) "two"
}
object(stdClass)#%d (1) {
  ["a"]=>
  string(5) "three"
}
==class24==
class24::__construct
object(class24)#%d (1) {
  ["a"]=>
  string(3) "one"
}
class24::__construct
object(class24)#%d (1) {
  ["a"]=>
  string(3) "two"
}
class24::__construct
object(class24)#%d (1) {
  ["a"]=>
  string(5) "three"
}
==DONE==
