--TEST--
enable local infile
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
if (!MYSQLPDOTest::isPDOMySQLnd())
	die("skip mysqlnd only test");
?>
--FILE--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'config.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$dsn = MySQLPDOTest::getDSN();
$user = PDO_MYSQL_TEST_USER;
$pass = PDO_MYSQL_TEST_PASS;

$db = new PDO($dsn, $user, $pass, array(PDO::MYSQL_ATTR_LOCAL_INFILE => true));
echo var_export($db->getAttribute(PDO::MYSQL_ATTR_LOCAL_INFILE)), "\n";
echo "done!\n";
?>
--EXPECTF--
true
done!
