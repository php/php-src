--TEST--
SQLite3::lastErrorCode test with parameters
--CREDITS--
Thijs Feryn <thijs@feryn.eu>
#TestFest PHPBelgium 2009
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$db = new SQLite3(':memory:');
var_dump($db->lastErrorCode('invalid argument'));
echo "Done\n";
?>
--EXPECTF--
Warning: SQLite3::lastErrorCode() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
