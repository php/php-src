--TEST--
Bug #73530 (Unsetting result set may reset other result set)
--SKIPIF--
<?php
if (!extension_loaded('sqlite3')) die('skip sqlite3 extension not available');
?>
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec("CREATE TABLE foo (num int)");
$db->exec("INSERT INTO foo VALUES (0)");
$db->exec("INSERT INTO foo VALUES (1)");
$stmt = $db->prepare("SELECT * FROM foo WHERE NUM = ?");
$stmt->bindValue(1, 0, SQLITE3_INTEGER);
$res1 = $stmt->execute();
$res1->finalize();
$stmt->clear();
$stmt->reset();
$stmt->bindValue(1, 1, SQLITE3_INTEGER);
$res2 = $stmt->execute();
while ($row = $res2->fetchArray(SQLITE3_ASSOC)) {
    var_dump($row);
    unset($res1);
}
?>
===DONE===
--EXPECT--
array(1) {
  ["num"]=>
  int(1)
}
===DONE===
