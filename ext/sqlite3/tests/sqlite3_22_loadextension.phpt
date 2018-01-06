--TEST--
SQLite3 load extension
--SKIPIF--
<?php
require_once(__DIR__ . '/skipif.inc');
$r = new ReflectionClass("sqlite3");
if (!$r->hasMethod("loadExtension")) {
	die("skip - sqlite3 doesn't have loadExtension enabled");
}
?>
--INI--
open_basedir=.
sqlite3.extension_dir=.
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$directory = __DIR__;

touch($directory . '/myext.txt');

var_dump($db->loadExtension('myext.txt'));
var_dump($db->close());
unlink($directory . '/myext.txt');

echo "Done\n";
?>
--EXPECTF--
Warning: SQLite3::loadExtension(): Unable to load extension at '.%emyext.txt' in %s on line %d
bool(false)
bool(true)
Done
