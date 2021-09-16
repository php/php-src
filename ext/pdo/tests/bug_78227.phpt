--TEST--
Bug #78227 Prepared statements ignore RENAME COLUMN
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
@$db->exec("drop table test");

$db->exec("CREATE TABLE test (id int, foo varchar(20))");

$stmt = $db->prepare("INSERT INTO test (id, foo) VALUES (?, ?)");
$stmt->execute([10, 'test']);

$stmt = $db->prepare("SELECT * FROM test WHERE id = :id");
$stmt->execute(['id' => 10]);
print_r($stmt->fetch(PDO::FETCH_ASSOC));

$driver = $db->getAttribute(PDO::ATTR_DRIVER_NAME);
switch(true){
    case "mysql" == $driver:
        $db->exec("ALTER TABLE test change foo bar varchar(20)");
        break;
    case "sqlite" == $driver:
        $db->exec("ALTER TABLE test rename column foo to bar");
        break;
    case "oci" == $driver:
    case "pgsql" == $driver:
        $db->exec("ALTER TABLE test rename column foo to bar");
        break;
    case "firebird" == $driver:
        $db->exec("ALTER TABLE test alter column foo to bar");
        break;
    case "mssql" == $driver:
    case "dblib" == $driver:
    case "odbc" == $driver:
        $db->exec("execute sp_rename 'test.foo','bar','column'");
        break;
    case "sybase" == $driver:
        $db->exec("sp_rename 'test.foo','bar','column'");
        break;
}

$res = $stmt->execute(['id' => 10]);
print_r($stmt->fetch(PDO::FETCH_ASSOC));
?>
--CLEAN--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
@$db->exec("drop table test");
?>
--EXPECT--
Array
(
    [id] => 10
    [foo] => test
)
Array
(
    [id] => 10
    [bar] => test
)
