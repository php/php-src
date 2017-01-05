--TEST--
SQLite3::open test, testing for function parameters
--CREDITS--
Felix De Vliegher
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

try {
  $db = new SQLite3();
} catch (TypeError $e) {
  var_dump($e->getMessage());
}

?>
--EXPECTF--
%string|unicode%(60) "SQLite3::__construct() expects at least 1 parameter, 0 given"
