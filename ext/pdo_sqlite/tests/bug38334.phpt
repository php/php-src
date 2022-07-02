--TEST--
Bug #38334: Proper data-type support for PDO_SQLITE
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');
$db->exec('CREATE TABLE test (i INTEGER , f DOUBLE, s VARCHAR(255))');
$db->exec('INSERT INTO test VALUES (42, 46.7, "test")');
var_dump($db->query('SELECT * FROM test')->fetch(PDO::FETCH_ASSOC));

// Check handling of integers larger than 32-bit.
$db->exec('INSERT INTO test VALUES (10000000000, 0.0, "")');
$i = $db->query('SELECT i FROM test WHERE f = 0.0')->fetchColumn(0);
if (PHP_INT_SIZE >= 8) {
    var_dump($i === 10000000000);
} else {
    var_dump($i === '10000000000');
}

// Check storing of strings into integer/float columns.
$db->exec('INSERT INTO test VALUES ("test", "test", "x")');
var_dump($db->query('SELECT * FROM test WHERE s = "x"')->fetch(PDO::FETCH_ASSOC));

?>
--EXPECT--
array(3) {
  ["i"]=>
  int(42)
  ["f"]=>
  float(46.7)
  ["s"]=>
  string(4) "test"
}
bool(true)
array(3) {
  ["i"]=>
  string(4) "test"
  ["f"]=>
  string(4) "test"
  ["s"]=>
  string(1) "x"
}
