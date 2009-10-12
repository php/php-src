--TEST--
Testing SQLiteDatabase::ArrayQuery with NULL-byte string
--SKIPIF--
<?php
if (!extension_loaded("sqlite")) print "skip";
?>
--FILE--
<?php

$method = new ReflectionMethod('sqlitedatabase::arrayquery');

$class = $method->getDeclaringClass()->newInstanceArgs(array(':memory:'));

$p = "\0";

$method->invokeArgs($class, array_fill(0, 2, $p));
$method->invokeArgs($class, array_fill(0, 1, $p));

?>
--EXPECTF--
Warning: SQLiteDatabase::arrayQuery() expects parameter 2 to be long, string given in %s on line %d

Warning: SQLiteDatabase::arrayQuery(): Cannot execute empty query. in %s on line %d
