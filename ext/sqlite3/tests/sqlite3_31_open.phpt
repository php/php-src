--TEST--
SQLite3::re-initialize object tests
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
--EXTENSIONS--
sqlite3
--FILE--
<?php

try {
  $db = new SQLite3(__DIR__ . '/db1.db');
  $db->open(__DIR__ . '/db1.db');
} catch (Exception $ex) {
  echo $ex::class, ': ', $ex->getMessage(), "\n";
}

?>
--CLEAN--
<?php @unlink(__DIR__ . '/db1.db'); ?>
--EXPECT--
Exception: Already initialised DB Object
