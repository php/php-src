--TEST--
Bug #79276 (PDO ignores any placeholder between two '\')
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip pdo extension not available');
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
if (!strncasecmp(getenv('PDOTEST_DSN'), 'mysql', strlen('mysql'))) {
    $db->exec("SET SESSION sql_mode = 'NO_BACKSLASH_ESCAPES'");
}

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);

$create_sql = "CREATE TABLE test (foo INT, bar VARCHAR(10))";
$db->exec($create_sql);

$insert_sql = "INSERT INTO test VALUES (1, '\\')";
$db->exec($insert_sql);

$sql = "SELECT * FROM test WHERE bar = '\\' AND foo = :foo AND bar = '\\'";
$sth = $db->prepare($sql);
$sth->execute(['foo' => 1]);
$row = $sth->fetch(PDO::FETCH_NUM);
var_dump($row);
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "\"
}
