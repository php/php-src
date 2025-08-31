--TEST--
PDO SQLite Bug #78192 SegFault when reuse statement after schema change
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$connection = new \PDO('sqlite::memory:');
$connection->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
$connection->query('CREATE TABLE user (id INTEGER PRIMARY KEY NOT NULL, name VARCHAR(255) NOT NULL)');

$stmt = $connection->prepare('INSERT INTO user (id, name) VALUES(:id, :name)');
$stmt->execute([
    'id'   => 10,
    'name' => 'test',
]);

$stmt = $connection->prepare('SELECT * FROM user WHERE id = :id');
$stmt->execute(['id' => 10]);
var_dump($stmt->fetchAll(\PDO::FETCH_ASSOC));

$connection->query('ALTER TABLE user ADD new_col VARCHAR(255)');
$stmt->execute(['id' => 10]);
var_dump($stmt->fetchAll(\PDO::FETCH_ASSOC));
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    int(10)
    ["name"]=>
    string(4) "test"
  }
}
array(1) {
  [0]=>
  array(3) {
    ["id"]=>
    int(10)
    ["name"]=>
    string(4) "test"
    ["new_col"]=>
    NULL
  }
}
