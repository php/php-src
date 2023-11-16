--TEST--
GH-11587 PHP8.1: Fixed the condition for result set values to be of native type, making it compatible with previous versions. #11622
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
if (!extension_loaded('mysqlnd')) die('skip: This test requires the loading of mysqlnd');
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();

$createTestTable = <<<SQL
CREATE TABLE test_11587(
   id INT,
  `float_col` FLOAT(3,2) DEFAULT NULL,
  `double_col` DOUBLE(3,2) DEFAULT NULL,
  `decimal_col` DECIMAL(3,2) DEFAULT NULL
)
SQL;

$db->exec($createTestTable);

$insertTestTable = <<<SQL
INSERT INTO test_11587(id, float_col, double_col, decimal_col) VALUES(1, 2.60, 3.60, 4.60)
SQL;

$db->exec($insertTestTable);

echo "PDO::ATTR_EMULATE_PREPARES = true, PDO::ATTR_STRINGIFY_FETCHES = true\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
$results = $db->query('SELECT * FROM test_11587');
foreach ($results as $result) {
    var_dump($result);
}

echo "\n";

echo "PDO::ATTR_EMULATE_PREPARES = true, PDO::ATTR_STRINGIFY_FETCHES = false\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);
$results = $db->query('SELECT * FROM test_11587');
foreach ($results as $result) {
    var_dump($result);
}

echo "\n";

echo "PDO::ATTR_EMULATE_PREPARES = false, PDO::ATTR_STRINGIFY_FETCHES = true\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
$results = $db->query('SELECT * FROM test_11587');
foreach ($results as $result) {
    var_dump($result);
}

echo "\n";

echo "PDO::ATTR_EMULATE_PREPARES = false, PDO::ATTR_STRINGIFY_FETCHES = false\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);
$results = $db->query('SELECT * FROM test_11587');
foreach ($results as $result) {
    var_dump($result);
}

echo "\n";

echo 'done!';
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_11587');
?>
--EXPECT--
PDO::ATTR_EMULATE_PREPARES = true, PDO::ATTR_STRINGIFY_FETCHES = true
array(8) {
  ["id"]=>
  string(1) "1"
  [0]=>
  string(1) "1"
  ["float_col"]=>
  string(4) "2.60"
  [1]=>
  string(4) "2.60"
  ["double_col"]=>
  string(4) "3.60"
  [2]=>
  string(4) "3.60"
  ["decimal_col"]=>
  string(4) "4.60"
  [3]=>
  string(4) "4.60"
}

PDO::ATTR_EMULATE_PREPARES = true, PDO::ATTR_STRINGIFY_FETCHES = false
array(8) {
  ["id"]=>
  int(1)
  [0]=>
  int(1)
  ["float_col"]=>
  float(2.6)
  [1]=>
  float(2.6)
  ["double_col"]=>
  float(3.6)
  [2]=>
  float(3.6)
  ["decimal_col"]=>
  string(4) "4.60"
  [3]=>
  string(4) "4.60"
}

PDO::ATTR_EMULATE_PREPARES = false, PDO::ATTR_STRINGIFY_FETCHES = true
array(8) {
  ["id"]=>
  string(1) "1"
  [0]=>
  string(1) "1"
  ["float_col"]=>
  string(3) "2.6"
  [1]=>
  string(3) "2.6"
  ["double_col"]=>
  string(3) "3.6"
  [2]=>
  string(3) "3.6"
  ["decimal_col"]=>
  string(4) "4.60"
  [3]=>
  string(4) "4.60"
}

PDO::ATTR_EMULATE_PREPARES = false, PDO::ATTR_STRINGIFY_FETCHES = false
array(8) {
  ["id"]=>
  int(1)
  [0]=>
  int(1)
  ["float_col"]=>
  float(2.6)
  [1]=>
  float(2.6)
  ["double_col"]=>
  float(3.6)
  [2]=>
  float(3.6)
  ["decimal_col"]=>
  string(4) "4.60"
  [3]=>
  string(4) "4.60"
}

done!
