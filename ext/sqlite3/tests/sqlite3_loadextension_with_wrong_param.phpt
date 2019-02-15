--TEST--
SQLite3::loadExtension test with wrong parameter type
--CREDITS--
Thijs Feryn <thijs@feryn.eu>
#TestFest PHPBelgium 2009
--SKIPIF--
<?php
require_once(__DIR__ . '/skipif.inc');

if (!method_exists('SQLite3', 'loadExtension')) {
	die("skip if SQLITE_OMIT_LOAD_EXTENSION defined");
}
?>
--FILE--
<?php
$db = new SQLite3(':memory:');
var_dump($db->loadExtension(array()));
echo "Done\n";
?>
--EXPECTF--
Warning: SQLite3::loadExtension() expects parameter 1 to be string, array given in %s on line %d
NULL
Done
