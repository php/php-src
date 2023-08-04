--TEST--
Bug #80908: pdo_mysql lastInsertId() return wrong, when table id bigger than the maximum value of int64
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

function createDB(): PDO {
    $db = MySQLPDOTest::factory();
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
    return $db;
}

$db = createDB();
$db->exec('CREATE TABLE test_80908 (`id` bigint(20) unsigned AUTO_INCREMENT, `name` varchar(5), primary key (`id`)) ENGINE = InnoDB AUTO_INCREMENT=10376293541461622799');

function testLastInsertId(PDO $db) {
    echo "Running test lastInsertId\n";
    $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);
    try {
        $db->exec("insert into test_80908 (`name`) values ('bar')");
        $id = $db->lastInsertId();
        echo "Last insert id is " . $id . "\n";
    } catch (PDOException $e) {
        echo $e->getMessage()."\n";
    }
}

testLastInsertId($db);
unset($db);
echo "\n";

?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_80908');
?>
--EXPECT--
Running test lastInsertId
Last insert id is 10376293541461622799
