--TEST--
PDO_sqlite: Testing DQS support
--EXTENSIONS--
pdo_sqlite
--SKIPIF--
<?php
$db = new PDO('sqlite::memory:');
try {
    $db->exec('SELECT "test"');
} catch (\PDOException) {
    die('skip SQLite is lacking DQS');
}
?>
--FILE--
<?php
$db = new PDO('sqlite::memory:');
$db->exec('CREATE TABLE test (s1 VARCHAR(255), s2 VARCHAR(255))');
$db->exec('INSERT INTO test VALUES (\'test\', "test")');
var_dump($db->query('SELECT * FROM test')->fetch(PDO::FETCH_ASSOC));
?>
--EXPECT--
array(2) {
  ["s1"]=>
  string(4) "test"
  ["s2"]=>
  string(4) "test"
}
