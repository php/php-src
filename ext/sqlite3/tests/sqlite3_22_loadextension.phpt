--TEST--
SQLite3 load extension
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
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

try {
	var_dump($db->loadExtension('myext.txt'));
}
catch (SQLite3Exception $e) {
	echo $e->getMessage() . "\n";
}
var_dump($db->close());
unlink($directory . '/myext.txt');

echo "Done\n";
?>
--EXPECTF--
Unable to load extension at '.%emyext.txt'
bool(true)
Done
