--TEST--
enable local infile
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
$user = PDO_MYSQL_TEST_USER;
$pass = PDO_MYSQL_TEST_PASS;

$db = new PDO($dsn, $user, $pass, array(PDO::MYSQL_ATTR_LOCAL_INFILE => true));
echo var_export($db->getAttribute(PDO::MYSQL_ATTR_LOCAL_INFILE)), "\n";
echo "done!\n";
?>
--EXPECT--
true
done!
