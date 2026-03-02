--TEST--
SQLite3Stmt::getSQL expanded test
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
if (SQLite3::version()['versionNumber'] < 3014000) {
    die('skip SQLite < 3.14 installed, requires SQLite >= 3.14');
}
?>
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$db->enableExceptions(true);

$stmt = $db->prepare('SELECT :a, :b, ?;');

$stmt->bindValue(':a', 42);
$stmt->bindValue(':b', 'php');
$stmt->bindValue(3, 43);

echo "Getting expanded SQL statement\n";
var_dump($stmt->getSQL(true));

echo "Execute statement\n";
var_dump($res = $stmt->execute());

echo "Statement result\n";
var_dump($res->fetchArray(SQLITE3_NUM));

$stmt->reset();

echo "Change binded values\n";
$stmt->bindValue(':a', 'TEST');
$stmt->bindValue(':b', '!!!');
$stmt->bindValue(3, 40);

echo "Getting expanded SQL statement\n";
var_dump($stmt->getSQL(true));

echo "Closing DB\n";
var_dump($db->close());

echo "Done\n";
?>
--EXPECT--
Getting expanded SQL statement
string(21) "SELECT 42, 'php', 43;"
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
Change binded values
Getting expanded SQL statement
string(25) "SELECT 'TEST', '!!!', 40;"
Closing DB
bool(true)
Done
