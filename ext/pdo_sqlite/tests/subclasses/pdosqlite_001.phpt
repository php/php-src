--TEST--
PdoSqlite basic
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PdoSqlite('sqlite::memory:');

$db->query('CREATE TABLE pdosqlite_001 (id INT AUTO INCREMENT, name TEXT)');

$db->query('INSERT INTO pdosqlite_001 VALUES (NULL, "PHP")');
$db->query('INSERT INTO pdosqlite_001 VALUES (NULL, "PHP6")');

$db->createFunction('testing', function($v) { return strtolower($v); }, 1, PdoSqlite::DETERMINISTIC);

foreach ($db->query('SELECT testing(name) FROM pdosqlite_001') as $row) {
    var_dump($row);
}

$db->query('DROP TABLE pdosqlite_001');

echo "Fin.";
?>
--EXPECT--
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
Fin.
