--TEST--
PDO MySQL Bug #33689 (query() execute() and fetch() return false on valid select queries)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();

$db->exec('CREATE TABLE test_33689 (bar INT NOT NULL)');
$db->exec('INSERT INTO test_33689 VALUES(1)');

var_dump($db->query('SELECT * FROM test_33689'));
foreach ($db->query('SELECT * FROM test_33689') as $row) {
    print_r($row);
}

$stmt = $db->prepare('SELECT * FROM test_33689');
print_r($stmt->getColumnMeta(0));
$stmt->execute();
$tmp = $stmt->getColumnMeta(0);

// libmysql and mysqlnd will show the pdo_type entry at a different position in the hash
// and will report a different type, as mysqlnd returns native types.
if (!isset($tmp['pdo_type']) || ($tmp['pdo_type'] != 1 && $tmp['pdo_type'] != 2))
    printf("Expecting pdo_type = 1 got %s\n", $tmp['pdo_type']);
else
    unset($tmp['pdo_type']);

print_r($tmp);
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_33689');
?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(24) "SELECT * FROM test_33689"
}
Array
(
    [bar] => 1
    [0] => 1
)
Array
(
    [native_type] => LONG
    [flags] => Array
        (
            [0] => not_null
        )

    [table] => test_33689
    [name] => bar
    [len] => 11
    [precision] => 0
)
