--TEST--
GH-20699 (SQLite3Result fetchArray return array|false, null returned)
--EXTENSIONS--
sqlite3
--CREDITS--
plusminmax
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->prepare('BEGIN;')->execute()->fetchArray();
?>
--EXPECTF--
Warning: SQLite3Result::fetchArray(): Unable to execute statement: cannot start a transaction within a transaction in %s on line %d
