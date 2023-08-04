--TEST--
Change column count after statement has been prepared
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

$db->exec('CREATE TABLE test_change_column_count (id INTEGER PRIMARY KEY NOT NULL, name VARCHAR(255) NOT NULL)');

$stmt = $db->prepare('INSERT INTO test_change_column_count (id, name) VALUES(:id, :name)');
$stmt->execute([
    'id'   => 10,
    'name' => 'test',
]);

$stmt = $db->prepare('SELECT * FROM test_change_column_count WHERE id = :id');
$stmt->execute(['id' => 10]);
var_dump($stmt->fetchAll(\PDO::FETCH_ASSOC));

$db->exec('ALTER TABLE test_change_column_count ADD new_col VARCHAR(255)');
$stmt->execute(['id' => 10]);
var_dump($stmt->fetchAll(\PDO::FETCH_ASSOC));

?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_change_column_count');
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(2) "10"
    ["name"]=>
    string(4) "test"
  }
}
array(1) {
  [0]=>
  array(3) {
    ["id"]=>
    string(2) "10"
    ["name"]=>
    string(4) "test"
    ["new_col"]=>
    NULL
  }
}
