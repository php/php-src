--TEST--
Bug #71049 (SQLite3Stmt::execute() releases bound parameter instead of internal buffer)
--EXTENSIONS--
sqlite3
--FILE--
<?php

require(__DIR__ . '/new_db.inc');

$db->exec('CREATE TABLE test (age INTEGER, id STRING)');

$stmt = $db->prepare("SELECT * FROM test WHERE id = ? ORDER BY id ASC");
$foo = "alive" . chr(33);
$stmt->bindParam(1, $foo, SQLITE3_BLOB);
$results = $stmt->execute();
var_dump($foo);
$db->close();
?>
--EXPECT--
string(6) "alive!"
