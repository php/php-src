--TEST--
PDO_sqlite: Testing createAggregate()
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

// This test was copied from the pdo_sqlite test for sqliteCreateAggregate
$db = new PdoSqlite('sqlite::memory:');

$db->query('CREATE TABLE foobar (id INT AUTO INCREMENT, name TEXT)');

$db->query('INSERT INTO foobar VALUES (NULL, "PHP")');
$db->query('INSERT INTO foobar VALUES (NULL, "PHP6")');

$db->createAggregate('testing', function(&$a, $b) { $a .= $b; return $a; }, function(&$v) { return $v; });


foreach ($db->query('SELECT testing(name) FROM foobar') as $row) {
    var_dump($row);
}

$db->query('DROP TABLE foobar');

?>
--EXPECT--
array(2) {
  ["testing(name)"]=>
  string(2) "12"
  [0]=>
  string(2) "12"
}
