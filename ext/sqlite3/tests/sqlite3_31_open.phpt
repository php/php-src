--TEST--
SQLite3::re-initialize object tests
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

try {
  $db = new SQLite3('db1.db');
  $db->open('db1.db');
} catch (Exception $ex) {
  var_dump($ex->getMessage());
}

?>
--EXPECTF--
%string|unicode%(29) "Already initialised DB Object"
