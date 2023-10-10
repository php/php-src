--TEST--
PDO Common: assert that bindParam does not modify parameter
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

switch ($db->getAttribute(PDO::ATTR_DRIVER_NAME)) {
    case 'dblib':
        // environment settings can influence how the table is created if specifics are missing
        // https://msdn.microsoft.com/en-us/library/ms174979.aspx#Nullability Rules Within a Table Definition
        $sql = 'create test024 (id int, name varchar(10) null)';
        break;
    default:
        $sql = 'create test024 (id int, name varchar(10))';
        break;
}
$db->exec($sql);

$stmt = $db->prepare('insert into test024 (id, name) values(0, :name)');
$name = NULL;
$before_bind = $name;
$stmt->bindParam(':name', $name, PDO::PARAM_NULL);
if ($name !== $before_bind) {
    echo "bind: fail\n";
} else {
    echo "bind: success\n";
}
var_dump($stmt->execute());
var_dump($db->query('select name from test024 where id=0')->fetchColumn());

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test024");
?>
--EXPECT--
bind: success
bool(true)
NULL
