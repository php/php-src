--TEST--
GH-11451 (Invalid associative array containing duplicate keys)
--EXTENSIONS--
sqlite3
--FILE--
<?php
var_dump((new SQLite3(':memory:'))
	->query('SELECT 1 AS key, 2 AS key')
	->fetchArray(SQLITE3_ASSOC));

var_dump((new SQLite3(':memory:'))
	->query('SELECT 0 AS dummy, 1 AS key, 2 AS key')
	->fetchArray(SQLITE3_ASSOC));
?>
--EXPECT--
array(1) {
  ["key"]=>
  int(2)
}
array(2) {
  ["dummy"]=>
  int(0)
  ["key"]=>
  int(2)
}
