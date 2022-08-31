--TEST--
Bug #79293 (SQLite3Result::fetchArray() may fetch rows after last row)
--SKIPIF--
<?php
if (!extension_loaded('sqlite3')) die('skip sqlite3 extension not available');
?>
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec("CREATE TABLE foo (bar INT)");
for ($i = 1; $i <= 3; $i++) {
    $db->exec("INSERT INTO foo VALUES ($i)");
}

$res = $db->query("SELECT * FROM foo");
while (($row = $res->fetchArray(SQLITE3_ASSOC))) {
    var_dump($row);
}
var_dump($res->fetchArray(SQLITE3_ASSOC));
?>
--EXPECT--
array(1) {
  ["bar"]=>
  int(1)
}
array(1) {
  ["bar"]=>
  int(2)
}
array(1) {
  ["bar"]=>
  int(3)
}
bool(false)
