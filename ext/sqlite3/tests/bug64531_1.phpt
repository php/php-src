--TEST--
Bug #64531 (SQLite3Result::fetchArray runs the query again) - SQLite3->query
--SKIPIF--
<?php
if (!extension_loaded('sqlite3')) die('skip sqlite3 extension not available');
?>
--FILE--
<?php
$conn = new SQLite3(':memory:');
$conn->exec("CREATE TABLE foo (id INT)");

$res = $conn->query("INSERT INTO foo VALUES (1)");

$res->fetchArray();
$res->fetchArray();

$res = $conn->query("SELECT * FROM foo");
while (($row = $res->fetchArray(SQLITE3_NUM))) {
    var_dump($row);
}
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
