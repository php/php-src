--TEST--
Bug #72571 (SQLite3::bindValue, SQLite3::bindParam crash)
--SKIPIF--
<?php
if (!extension_loaded('sqlite3')) die('skip'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');

$stmt = $db->prepare("select 1 = ?");

// bindParam crash
$i = 0;
$stmt->bindParam(0, $i);

var_dump($stmt->execute());
$db->close();
?>
--EXPECTF--
object(SQLite3Result)#%d (0) {
}
