--TEST--
Bug #43371 (Memory errors in PDO constructor)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$dsn = MySQLPDOTest::getDSN();
$db = new PDO($dsn, PDO_MYSQL_TEST_USER, PDO_MYSQL_TEST_PASS, array(PDO::ATTR_PERSISTENT => true));

print "done!";
?>
--EXPECT--
done!
