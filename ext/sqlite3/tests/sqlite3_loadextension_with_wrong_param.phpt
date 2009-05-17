--TEST--
SQLite3::loadExtension test with wrong parameter type
--CREDITS--
Thijs Feryn <thijs@feryn.eu>
#TestFest PHPBelgium 2009
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');
var_dump($db->loadExtension(array()));
echo "Done\n";
?>
--EXPECTF--
Warning: SQLite3::loadExtension() expects parameter 1 to be %binary_string_optional%, array given in %s on line %d
NULL
Done

