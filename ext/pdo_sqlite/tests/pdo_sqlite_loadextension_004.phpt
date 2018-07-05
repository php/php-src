--TEST--
PDO_sqlite: Testing sqliteLoadExtension() with wrong parameter type
--CREDITS--
Thijs Feryn <thijs@feryn.eu>
#TestFest PHPBelgium 2009
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php
$db = new pdo('sqlite::memory:');
var_dump($db->sqliteLoadExtension(array()));
echo "Done\n";
?>
--EXPECTF--
Warning: PDO::sqliteLoadExtension() expects parameter 1 to be string, array given in %s on line %d
bool(false)
Done

