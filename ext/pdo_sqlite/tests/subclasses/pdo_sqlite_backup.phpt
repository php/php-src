--TEST--
Pdo\Sqlite::backup test
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php

$db = Pdo::connect('sqlite::memory:');

$db->exec('CREATE TABLE test (a, b);');
$db->exec('INSERT INTO test VALUES (42, \'php\');');

$db2 = Pdo::connect('sqlite::memory:');

echo "Backup to DB2\n";
var_dump($db->backup($db2));

echo "Checking if table has been copied\n";
var_dump($db2->query('SELECT COUNT(*) FROM sqlite_master;')->fetchAll());

echo "Checking backup contents\n";
var_dump($db2->query('SELECT a FROM test;')->fetchAll());
var_dump($db2->query('SELECT b FROM test;')->fetchAll());

echo "Resetting DB2\n";

unset($db2);
$db2 = Pdo::connect('sqlite::memory:');

echo "Locking DB1\n";
var_dump($db->exec('BEGIN EXCLUSIVE;'));

echo "Backup to DB2 (should fail)\n";
try {
    $db->backup($db2);
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
var_dump($db->backup($db2));

?>
--EXPECTF--
Backup to DB2
bool(true)
Checking if table has been copied
array(1) {
  [0]=>
  array(2) {
    ["COUNT(*)"]=>
    int(1)
    [0]=>
    int(1)
  }
}
Checking backup contents
array(1) {
  [0]=>
  array(2) {
    ["a"]=>
    int(42)
    [0]=>
    int(42)
  }
}
array(1) {
  [0]=>
  array(2) {
    ["b"]=>
    string(3) "php"
    [0]=>
    string(3) "php"
  }
}
Resetting DB2
Locking DB1
int(1)
Backup to DB2 (should fail)
SQLSTATE[HY000]: General error: Backup failed: source database is busy

Warning: Pdo\Sqlite::backup(): SQLSTATE[HY000]: General error: Backup failed: source database is busy in %s on line %d
bool(false)
