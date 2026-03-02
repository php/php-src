--TEST--
PDO Common: Bug #43130 (Bound parameters cannot have - in their name)
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
if (!strncasecmp(getenv('PDOTEST_DSN'), 'sqlite', strlen('sqlite'))) die('skip not relevant for sqlite driver');
if (!strncasecmp(getenv('PDOTEST_DSN'), 'pgsql', strlen('pgsql'))) die('skip not relevant for pgsql driver');
if (!strncasecmp(getenv('PDOTEST_DSN'), 'oci', strlen('oci'))) die('skip not relevant for oci driver - Hyphen is not legal for bind names in Oracle DB');
if (!strncasecmp(getenv('PDOTEST_DSN'), 'firebird', strlen('firebird'))) die('skip not relevant for firebird driver');
if (!strncasecmp(getenv('PDOTEST_DSN'), 'odbc', strlen('odbc'))) die('skip not relevant for odbc driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'mysql')
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);

$db->exec("CREATE TABLE test43130 (a varchar(100), b varchar(100), c varchar(100))");

for ($i = 0; $i < 5; $i++) {
    $db->exec("INSERT INTO test43130 (a,b,c) VALUES('test".$i."','".$i."','".$i."')");
}

$stmt = $db->prepare("SELECT a FROM test43130 WHERE b=:id-value");
$stmt->bindParam(':id-value', $id);
$id = '1';
$stmt->execute();
var_dump($stmt->fetch(PDO::FETCH_COLUMN));
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test43130");
?>
--EXPECTF--
Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: parameter was not defined in %s on line %d
bool(false)
