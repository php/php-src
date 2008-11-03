--TEST--
Bug #44327.2 (PDORow::queryString property & numeric offsets / Crash)
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php

$db = new pdo('sqlite:memory');

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
  [u"queryString"]=>
  unicode(23) "select 1 as queryString"
}
unicode(23) "select 1 as queryString"
array(2) {
  [u"queryString"]=>
  unicode(1) "1"
  [0]=>
  unicode(1) "1"
}
NULL
--------------------------------------------
object(PDOStatement)#%d (1) {
  [u"queryString"]=>
  unicode(23) "select 1 as queryString"
}
unicode(23) "select 1 as queryString"
object(PDORow)#%d (1) {
  [u"queryString"]=>
  unicode(1) "1"
}
unicode(1) "1"
