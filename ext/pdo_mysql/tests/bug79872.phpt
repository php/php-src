--TEST--
Bug #79872: Can't execute query with pending result sets
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$stmt = $db->prepare('SET @foo = 1; SET @bar = 2;');
$stmt->execute();
try {
    var_dump($db->query('SELECT @foo')->fetchAll());
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
SQLSTATE[HY000]: General error: 2014 Cannot execute queries while there are pending result sets. Consider unsetting the previous PDOStatement or calling PDOStatement::closeCursor()
