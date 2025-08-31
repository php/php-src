--TEST--
Bug #77051 SQLite3::bindParam memory bug when missing ::reset call
--EXTENSIONS--
sqlite3
--FILE--
<?php

$db = new SQLite3(':memory:');
$db->enableExceptions(true);

$stmt = $db->prepare('SELECT :a, :b, ?;');

$a = 42;
$stmt->bindParam(':a', $a, SQLITE3_INTEGER);
$stmt->bindValue(':b', 'php');
$stmt->bindValue(':b', 'PHP');
$stmt->bindValue(3, 424242);

echo "Execute statement\n";
var_dump($res = $stmt->execute());

echo "Statement result\n";
var_dump($res->fetchArray(SQLITE3_NUM));

echo "Change binded param to wrong type\n";
$a = 'TEST';

echo "Execute statement\n";
var_dump($res = $stmt->execute());

echo "Statement result\n";
var_dump($res->fetchArray(SQLITE3_NUM));

echo "Change binded values\n";
$a = 5252552;
$stmt->bindValue(':b', 'TEST');
$stmt->bindValue(3, '!!!');

echo "Execute statement\n";
var_dump($res = $stmt->execute());

echo "Statement result\n";
var_dump($res->fetchArray(SQLITE3_NUM));

?>
--EXPECT--
Execute statement
object(SQLite3Result)#3 (0) {
}
Statement result
array(3) {
  [0]=>
  int(42)
  [1]=>
  string(3) "PHP"
  [2]=>
  int(424242)
}
Change binded param to wrong type
Execute statement
object(SQLite3Result)#4 (0) {
}
Statement result
array(3) {
  [0]=>
  int(0)
  [1]=>
  string(3) "PHP"
  [2]=>
  int(424242)
}
Change binded values
Execute statement
object(SQLite3Result)#3 (0) {
}
Statement result
array(3) {
  [0]=>
  int(5252552)
  [1]=>
  string(4) "TEST"
  [2]=>
  string(3) "!!!"
}