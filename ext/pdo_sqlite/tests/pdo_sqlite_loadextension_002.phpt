--TEST--
PDO_sqlite: Testing sqliteLoadExtension() with empty extension test
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
--INI--
sqlite3.extension_dir=/tmp
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php

$db = new pdo('sqlite::memory:');

try {
  $db->sqliteLoadExtension("");
} catch (Extension $ex) {
  var_dump($ex->getMessage());
}

?>
--EXPECTF--
Warning: PDO::sqliteLoadExtension(): Empty string as an extension in %s on line %d
