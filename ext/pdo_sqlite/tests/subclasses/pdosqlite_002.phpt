--TEST--
PdoSqlite create through PDO::connect and function define.
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = Pdo::connect('sqlite::memory:');
if (!$db instanceof PdoSqlite) {
    echo "Wrong class type. Should be PdoSqlite but is " . get_class($db) . "\n";
}

$db->query('CREATE TABLE pdosqlite_002 (id INT AUTO INCREMENT, name TEXT)');
$db->query('INSERT INTO pdosqlite_002 VALUES (NULL, "PHP")');
$db->query('INSERT INTO pdosqlite_002 VALUES (NULL, "PHP6")');

$db->createFunction('testing', function($v) { return strtolower($v); }, 1, PdoSqlite::DETERMINISTIC);

foreach ($db->query('SELECT testing(name) FROM pdosqlite_002') as $row) {
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
