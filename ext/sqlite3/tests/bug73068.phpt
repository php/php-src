--TEST--
Bug #73068 SQLite3 may fail to DELETE
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$db = new \SQLite3(':memory:') ;

$db->exec("CREATE TABLE IF NOT EXISTS t1(a INT UNIQUE, b INT)");
$db->exec("INSERT OR REPLACE INTO t1(a,b) VALUES('1','2')");

$r = $db->query("SELECT * FROM t1 WHERE a='1' AND b='2'");
var_dump($r->fetchArray());

$db->exec("DELETE FROM t1 WHERE a='1' AND b='2'");

$r = $db->query("SELECT * FROM t1;");
var_dump($r->fetchArray());

$db->close();

?>
==OK==
--EXPECT--
array(4) {
  [0]=>
  int(1)
  ["a"]=>
  int(1)
  [1]=>
  int(2)
  ["b"]=>
  int(2)
}
bool(false)
==OK==
