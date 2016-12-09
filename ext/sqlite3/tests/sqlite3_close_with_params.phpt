--TEST--
SQLite3::close test with parameters
--CREDITS--
Thijs Feryn <thijs@feryn.eu>
#TestFest PHPBelgium 2009
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

$db = new SQLite3(':memory:');
var_dump($db->close('invalid argument'));
echo "Done\n";
?>
--EXPECTF--
Warning: SQLite3::close() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
