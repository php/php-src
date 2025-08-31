--TEST--
SQLite3::open test, testing for function parameters
--CREDITS--
Felix De Vliegher
# Belgian PHP Testfest 2009
--EXTENSIONS--
sqlite3
--FILE--
<?php

try {
  $db = new SQLite3();
} catch (TypeError $e) {
  var_dump($e->getMessage());
}

?>
--EXPECT--
string(59) "SQLite3::__construct() expects at least 1 argument, 0 given"
