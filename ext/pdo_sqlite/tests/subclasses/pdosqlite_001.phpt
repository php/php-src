--TEST--
Pdo\Sqlite basic
--EXTENSIONS--
pdo_sqlite
--SKIPIF--
<?php
if (!defined('Pdo\Sqlite::DETERMINISTIC')) die('skip Pdo\Sqlite::DETERMINISTIC requires SQLite library >= 3.8.3');
?>
--FILE--
<?php

$db = new Pdo\Sqlite('sqlite::memory:');

$db->query('CREATE TABLE pdosqlite_001 (id INT AUTO INCREMENT, name TEXT)');

$db->query('INSERT INTO pdosqlite_001 VALUES (NULL, "PHP")');
$db->query('INSERT INTO pdosqlite_001 VALUES (NULL, "PHP6")');

$db->createFunction('testing', function($v) { return strtolower($v); }, 1, Pdo\Sqlite::DETERMINISTIC);

foreach ($db->query('SELECT testing(name) FROM pdosqlite_001') as $row) {
    var_dump($row);
}

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
