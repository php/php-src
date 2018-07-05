--TEST--
PDO_sqlite: Testing sqliteLoadExtension()
--SKIPIF--
<?php
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
?>
--INI--
open_basedir=.
pdo_sqlite.extension_dir=.
--FILE--
<?php

$db = new pdo('sqlite::memory:');

$directory = __DIR__;

touch($directory . '/myext.txt');

var_dump($db->sqliteLoadExtension('myext.txt'));
var_dump($db->close());
unlink($directory . '/myext.txt');

echo "Done\n";
?>
--EXPECTF--
Warning: PDO::sqliteLoadExtension(): Unable to load extension at '.%emyext.txt' in %s on line %d
bool(false)
bool(true)
Done
