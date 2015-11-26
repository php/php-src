--TEST--
SQLite3::loadExtension with empty extension test
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
--INI--
sqlite3.extension_dir=/tmp
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$db = new SQLite3(':memory:');

try {
  $db->loadExtension("");
} catch (Extension $ex) {
  var_dump($ex->getMessage());
}

?>
--EXPECTF--
Warning: SQLite3::loadExtension(): Empty string as an extension in %s on line %d
