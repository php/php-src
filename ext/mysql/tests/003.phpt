--TEST--
mysql_fetch_object
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include_once('connect.inc');

class class24 {
	function __construct() {
		echo __METHOD__ . "\n";
	}
}

$data = array("one", "two", "three");

if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
	printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);

if (!mysql_query('DROP TABLE IF EXISTS test', $link))
	printf("[002] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!mysql_query("CREATE TABLE test(a varchar(10))", $link))
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));

foreach ($data as $str) {
	if (!mysql_query(sprintf("INSERT INTO test VALUES('%s')", $str), $link))
		printf("[004 - %s] [%d] %s\n", $str, mysql_errno($link), mysql_error($link));
}

echo "==stdClass==\n";
if (!$res = mysql_query("SELECT a FROM test", $link))
	printf("[005] [%d] %s\n", mysql_errno($link), mysql_error($link));

while ($obj = mysql_fetch_object($res)) {
	var_dump($obj);
}
mysql_free_result($res);

echo "==class24==\n";
if (!$res = mysql_query("SELECT a FROM test", $link))
    printf("[006] [%d] %s\n", mysql_errno($link), mysql_error($link));

while ($obj = mysql_fetch_object($res, 'class24')) {
	var_dump($obj);
}
mysql_free_result($res);
mysql_close($link);
print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
==stdClass==
object(stdClass)#%d (1) {
  [%u|b%"a"]=>
  %unicode|string%(3) "one"
}
object(stdClass)#%d (1) {
  [%u|b%"a"]=>
  %unicode|string%(3) "two"
}
object(stdClass)#%d (1) {
  [%u|b%"a"]=>
  %unicode|string%(5) "three"
}
==class24==
class24::__construct
object(class24)#%d (1) {
  [%u|b%"a"]=>
  %unicode|string%(3) "one"
}
class24::__construct
object(class24)#%d (1) {
  [%u|b%"a"]=>
  %unicode|string%(3) "two"
}
class24::__construct
object(class24)#%d (1) {
  [%u|b%"a"]=>
  %unicode|string%(5) "three"
}
done!
