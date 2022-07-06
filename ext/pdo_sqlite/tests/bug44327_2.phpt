--TEST--
Bug #44327.2 (PDORow::queryString property & numeric offsets / Crash)
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php

$db = new PDO('sqlite::memory:');

$x = $db->query('select 1 as queryString');
var_dump($x, $x->queryString);

$y = $x->fetch();
var_dump($y, @$y->queryString);

print "--------------------------------------------\n";

$x = $db->query('select 1 as queryString');
var_dump($x, $x->queryString);

$y = $x->fetch(PDO::FETCH_LAZY);
var_dump($y, $y->queryString);

?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(23) "select 1 as queryString"
}
string(23) "select 1 as queryString"
array(2) {
  ["queryString"]=>
  string(1) "1"
  [0]=>
  string(1) "1"
}
NULL
--------------------------------------------
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(23) "select 1 as queryString"
}
string(23) "select 1 as queryString"
object(PDORow)#%d (1) {
  ["queryString"]=>
  string(1) "1"
}
string(1) "1"
