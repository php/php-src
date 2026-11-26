--TEST--
GH-25809 (SQLite3::backup() returns true when the backup fails)
--EXTENSIONS--
sqlite3
--FILE--
<?php

$source = new SQLite3(':memory:');
$source->exec('CREATE TABLE t (a); INSERT INTO t VALUES (1)');
$destination = new SQLite3(':memory:');

var_dump($source->backup($destination, 'main', 'nonexistent'));
var_dump($source->backup($destination, 'nonexistent', 'main'));

$file = __DIR__ . '/gh25809.db';
@unlink($file);
$writer = new SQLite3($file);
$writer->exec('CREATE TABLE z (a)');
$writer->close();
$readonly = new SQLite3($file, SQLITE3_OPEN_READONLY);
var_dump($source->backup($readonly));

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25809.db');
?>
--EXPECTF--
Warning: SQLite3::backup(): Backup failed: unknown database nonexistent in %s on line %d
bool(false)

Warning: SQLite3::backup(): Backup failed: unknown database nonexistent in %s on line %d
bool(false)

Warning: SQLite3::backup(): Backup failed: attempt to write a readonly database in %s on line %d
bool(false)
