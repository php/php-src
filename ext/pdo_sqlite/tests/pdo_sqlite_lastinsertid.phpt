--TEST--
PDO_sqlite: Testing lastInsertId()
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');
$db->query('CREATE TABLE test_pdo_sqlite_lastinsertid (id INT AUTO INCREMENT, name TEXT)');
$db->query('INSERT INTO test_pdo_sqlite_lastinsertid VALUES (NULL, "PHP"), (NULL, "PHP6")');
var_dump($db->query('SELECT * FROM test_pdo_sqlite_lastinsertid'));
var_dump($db->errorInfo());
var_dump($db->lastInsertId());

?>
--EXPECT--
object(PDOStatement)#2 (1) {
  ["queryString"]=>
  string(42) "SELECT * FROM test_pdo_sqlite_lastinsertid"
}
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
string(1) "2"
