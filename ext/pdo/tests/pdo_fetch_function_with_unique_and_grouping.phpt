--TEST--
PDO Common: PDO::FETCH_FUNC with a simple callback and uniqueness+grouping (PDO::FETCH_FUNC|PDO::FETCH_UNIQUE|PDO::FETCH_GROUP)
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec('CREATE TABLE pdo_fetch_function_with_uniqueness_and_grouping(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10), grp VARCHAR(10))');
// Firebird does not allow inserting multiple rows with INSERT INTO
$db->exec("INSERT INTO pdo_fetch_function_with_uniqueness_and_grouping VALUES (1, 'A', 'alpha', 'Group1')");
$db->exec("INSERT INTO pdo_fetch_function_with_uniqueness_and_grouping VALUES (2, 'B', 'beta', 'Group1')");
$db->exec("INSERT INTO pdo_fetch_function_with_uniqueness_and_grouping VALUES (3, 'C', 'gamma', 'Group2')");
$db->exec("INSERT INTO pdo_fetch_function_with_uniqueness_and_grouping VALUES (4, 'D', 'delta', 'Group2')");

function to_upper_with_log(string $str): string {
    echo __FUNCTION__, '(', var_export($str, true), ')', PHP_EOL;
    return strtoupper($str);
}

$pdoQuery = $db->prepare('SELECT grp, val2, val1 FROM pdo_fetch_function_with_uniqueness_and_grouping');
$pdoQuery->execute();
$result = $pdoQuery->fetchAll(PDO::FETCH_FUNC|PDO::FETCH_UNIQUE|PDO::FETCH_GROUP, 'to_upper_with_log');
var_dump($result);

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_function_with_uniqueness_and_grouping");
?>
--EXPECT--
to_upper_with_log('alpha')
to_upper_with_log('beta')
to_upper_with_log('gamma')
to_upper_with_log('delta')
array(2) {
  ["Group1"]=>
  string(4) "BETA"
  ["Group2"]=>
  string(5) "DELTA"
}
