--TEST--
SQLite3::re-initialize object tests
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

try {
  $db = new SQLite3(__DIR__ . '/db1.db');
  $db->open(__DIR__ . '/db1.db');
} catch (Exception $ex) {
  var_dump($ex->getMessage());
}

?>
--CLEAN--
<?php @unlink(__DIR__ . '/db1.db'); ?>
--EXPECT--
string(29) "Already initialised DB Object"
