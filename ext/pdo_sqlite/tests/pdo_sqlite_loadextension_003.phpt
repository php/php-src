--TEST--
PDO_sqlite: Testing sqliteLoadExtension() with disabled extensions
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
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
Warning: PDO::sqliteLoadExtension(): SQLite Extension are disabled in %s on line %d
