--TEST--
SQLite3::open test, testing for function parameters
--CREDITS--
Felix De Vliegher
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

try {
  $db = new SQLite3();
} catch (TypeError $e) {
  var_dump($e->getMessage());
}

?>
--EXPECT--
string(64) "SQLite3::__construct(): At least 1 argument is expected, 0 given"
