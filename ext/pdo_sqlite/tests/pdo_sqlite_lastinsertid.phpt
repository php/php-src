--TEST--
PDO_sqlite: Testing lastInsertId()
--FILE--
<?php

$db = new pdo('sqlite::memory:');
$db->query('CREATE TABLE IF NOT EXISTS foo (id INT AUTO INCREMENT, name TEXT)');
$db->query('INSERT INTO foo VALUES (NULL, "PHP")');
$db->query('INSERT INTO foo VALUES (NULL, "PHP6")');
var_dump($db->query('SELECT * FROM foo'));
var_dump($db->errorInfo());
var_dump($db->lastInsertId());

$db->query('DROP TABLE foo');

?>
--EXPECTF--
object(PDOStatement)#2 (1) {
  ["queryString"]=>
  %string|unicode%(17) "SELECT * FROM foo"
}
array(3) {
  [0]=>
  %string|unicode%(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
%string|unicode%(1) "2"
