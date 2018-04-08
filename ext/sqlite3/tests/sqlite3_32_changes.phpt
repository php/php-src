--TEST--
SQLite3::changes empty str tests
--CREDITS--
Ward Hus
#@ PHP TESTFEST 2009 (BELGIUM)
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');
//$db = new SQLite3('mysqlitedb.db');
$db->exec('CREATE TABLE pageView(id INTEGER PRIMARY KEY, page CHAR(256), access INTEGER(10))');
$db->exec('INSERT INTO pageView (page, access) VALUES (\'test\', \'000000\')');
echo $db->changes("dummy");
?>
--EXPECTF--
Warning: SQLite3::changes() expects exactly 0 parameters, 1 given in %s on line %d
