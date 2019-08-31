--TEST--
PDO::ATTR_PREFETCH
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
	require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();
	var_dump($db->getAttribute(PDO::ATTR_PREFETCH));
	var_dump($db->setAttribute(PDO::ATTR_PREFETCH, true));
	print "done!";
--EXPECTF--
Warning: PDO::getAttribute(): SQLSTATE[IM001]: Driver does not support this function: driver does not support that attribute in %s on line %d
bool(false)
bool(false)
done!
