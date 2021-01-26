--TEST--
ensure default for local infile is off
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'config.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$dsn = MySQLPDOTest::getDSN();
$user = PDO_MYSQL_TEST_USER;
$pass = PDO_MYSQL_TEST_PASS;

$db = new PDO($dsn, $user, $pass);
echo var_export($db->getAttribute(PDO::MYSQL_ATTR_LOCAL_INFILE)), "\n";
echo "done!\n";
?>
--EXPECT--
false
done!
