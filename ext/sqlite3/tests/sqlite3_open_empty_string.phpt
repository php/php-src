--TEST--
SQLite3::open test with empty string argument via the constructor
--CREDITS--
Thijs Feryn <thijs@feryn.eu>
#TestFest PHPBelgium 2009
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
$db = new SQLite3('');
echo "Done\n";
?>
--EXPECT--
Done
