--TEST--
SQLite3 defensive mode ini setting
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc');

if (SQLite3::version()['versionNumber'] < 3026000) {
    die("skip: sqlite3 library version < 3.26: no support for defensive mode");
}

?>
--INI--
sqlite3.defensive=On
--FILE--
<?php

$db = new SQLite3(':memory:');
var_dump($db->exec('CREATE TABLE test (a, b);'));

// This does not generate an error!
var_dump($db->exec('PRAGMA writable_schema = ON;'));
var_dump($db->querySingle('PRAGMA writable_schema;'));

// Should be 1
var_dump($db->querySingle('SELECT COUNT(*) FROM sqlite_master;'));

// Should generate an error!
var_dump($db->querySingle('DELETE FROM sqlite_master;'));

// Should still be 1
var_dump($db->querySingle('SELECT COUNT(*) FROM sqlite_master;'));
?>
--EXPECTF--
bool(true)
bool(true)
int(1)
int(1)

Warning: SQLite3::querySingle(): Unable to prepare statement: 1, table sqlite_master may not be modified in %s on line %d
bool(false)
int(1)