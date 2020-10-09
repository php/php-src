--TEST--
SQLite3Stmt::getSQL test
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$db->enableExceptions(true);

$stmt = $db->prepare('SELECT :a, :b, ?;');

$stmt->bindValue(':a', 42);
$stmt->bindValue(':b', 'php');
$stmt->bindValue(3, 43);

echo "Getting non-expanded SQL statement\n";
var_dump($stmt->getSQL(false));

echo "Execute statement\n";
var_dump($res = $stmt->execute());

echo "Statement result\n";
var_dump($res->fetchArray(SQLITE3_NUM));

echo "Closing DB\n";
var_dump($db->close());

echo "Done\n";
?>
--EXPECT--
Getting non-expanded SQL statement
string(17) "SELECT :a, :b, ?;"
Execute statement
object(SQLite3Result)#3 (0) {
}
Statement result
array(3) {
  [0]=>
  int(42)
  [1]=>
  string(3) "php"
  [2]=>
  int(43)
}
Closing DB
bool(true)
Done
