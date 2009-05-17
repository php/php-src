--TEST--
SQLite3::loadExtension with disabled extensions
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
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
Warning: SQLite3::loadExtension(): SQLite Extension are disabled in %s on line %d
