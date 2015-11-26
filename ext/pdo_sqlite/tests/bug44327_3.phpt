--TEST--
Bug #44327.3 (PDORow::queryString property & numeric offsets / Crash)
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php

$db = new pdo('sqlite::memory:');

$x = $db->query('select 1 as queryStringxx');
$y = $x->fetch(PDO::FETCH_LAZY);
var_dump($y, $y->queryString, $y->queryStringzz, $y->queryStringxx);

print "---\n";

var_dump($y[5], $y->{3});

?>
--EXPECTF--
object(PDORow)#%d (2) {
  ["queryString"]=>
  string(25) "select 1 as queryStringxx"
  ["queryStringxx"]=>
  string(1) "1"
}
string(25) "select 1 as queryStringxx"
NULL
string(1) "1"
---
NULL
NULL
