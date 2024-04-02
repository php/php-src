--TEST--
PDO Common: Bug #73234 (Emulated statements let value dictate parameter type)
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
if (str_starts_with(getenv('PDOTEST_DSN'), "firebird")) die('xfail firebird driver does not behave as expected');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();

$db = PDOTest::factory();
if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'oci') {
    die("xfail PDO::PARAM_NULL is not honored by OCI driver, related with bug #81586");
}
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$db = PDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);

switch ($db->getAttribute(PDO::ATTR_DRIVER_NAME)) {
    case 'dblib':
        $sql = 'CREATE TABLE test73234(id INT NULL)';
        break;
    default:
        $sql = 'CREATE TABLE test73234(id INT)';
        break;
}
$db->exec($sql);

$stmt = $db->prepare('INSERT INTO test73234 VALUES(:value)');

$stmt->bindValue(':value', 0, PDO::PARAM_NULL);
$stmt->execute();

$stmt->bindValue(':value', null, PDO::PARAM_NULL);
$stmt->execute();

$stmt = $db->query('SELECT * FROM test73234');
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test73234");
?>
--EXPECT--
array(2) {
  [0]=>
  array(1) {
    ["id"]=>
    NULL
  }
  [1]=>
  array(1) {
    ["id"]=>
    NULL
  }
}
