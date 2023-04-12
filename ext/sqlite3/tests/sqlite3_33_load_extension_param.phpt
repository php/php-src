--TEST--
SQLite3::loadExtension with empty extension test
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
--INI--
sqlite3.extension_dir="{TMP}"
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
if (!method_exists('SQLite3', 'loadExtension')) {
    die("skip if SQLITE_OMIT_LOAD_EXTENSION defined");
}
?>
--FILE--
<?php

$db = new SQLite3(':memory:');

try {
  $db->loadExtension("");
} catch (\Throwable $ex) {
  var_dump($ex->getMessage());
}

?>
--EXPECTF--
string(61) "SQLite3::loadExtension(): Argument #1 ($name) cannot be empty"
