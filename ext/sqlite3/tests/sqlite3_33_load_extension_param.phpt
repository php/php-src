--TEST--
SQLite3::loadExtension with empty extension test
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
--INI--
sqlite3.extension_dir="{TMP}"
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
$db->loadExtension("");

?>
--EXPECTF--
Warning: SQLite3::loadExtension(): Empty string as an extension in %s on line %d
