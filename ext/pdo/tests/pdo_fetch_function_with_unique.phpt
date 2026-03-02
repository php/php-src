--TEST--
PDO Common: PDO::FETCH_FUNC with a simple callback and uniqueness (PDO::FETCH_FUNC|PDO::FETCH_UNIQUE)
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

$db->exec('CREATE TABLE pdo_fetch_function_with_uniqueness(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_function_with_uniqueness VALUES (1, 'A', 'alpha')");
$db->exec("INSERT INTO pdo_fetch_function_with_uniqueness VALUES (2, 'B', 'beta')");
$db->exec("INSERT INTO pdo_fetch_function_with_uniqueness VALUES (3, 'C', 'gamma')");
$db->exec("INSERT INTO pdo_fetch_function_with_uniqueness VALUES (4, 'D', 'delta')");

function to_upper_with_log(string $str): string {
    echo __FUNCTION__, '(', var_export($str, true), ')', PHP_EOL;
    return strtoupper($str);
}

$pdoQuery = $db->prepare('SELECT val1, val2 FROM pdo_fetch_function_with_uniqueness');
$pdoQuery->execute();
$result = $pdoQuery->fetchAll(PDO::FETCH_FUNC|PDO::FETCH_UNIQUE, 'to_upper_with_log');
var_dump($result);

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_function_with_uniqueness");
?>
--EXPECT--
to_upper_with_log('alpha')
to_upper_with_log('beta')
to_upper_with_log('gamma')
to_upper_with_log('delta')
array(4) {
  ["A"]=>
  string(5) "ALPHA"
  ["B"]=>
  string(4) "BETA"
  ["C"]=>
  string(5) "GAMMA"
  ["D"]=>
  string(5) "DELTA"
}
