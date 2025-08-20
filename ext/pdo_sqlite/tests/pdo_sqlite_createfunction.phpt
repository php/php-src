--TEST--
PDO_sqlite: Testing sqliteCreateFunction()
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');

$db->query('CREATE TABLE test_pdo_sqlite_createfunction (id INT AUTO INCREMENT, name TEXT)');

$db->query('INSERT INTO test_pdo_sqlite_createfunction VALUES (NULL, "PHP"), (NULL, "PHP6")');


$db->sqliteCreateFunction('testing', function($v) { return strtolower($v); });


foreach ($db->query('SELECT testing(name) FROM test_pdo_sqlite_createfunction') as $row) {
    var_dump($row);
}

?>
--EXPECTF--
Deprecated: Method PDO::sqliteCreateFunction() is deprecated since 8.5, use Pdo\Sqlite::createFunction() instead in %s on line %d
array(2) {
  ["testing(name)"]=>
  string(3) "php"
  [0]=>
  string(3) "php"
}
array(2) {
  ["testing(name)"]=>
  string(4) "php6"
  [0]=>
  string(4) "php6"
}
