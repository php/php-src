--TEST--
MySQL PDO phpinfo() output
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();

	ob_start();
	phpinfo();
	$tmp = ob_get_contents();
	ob_end_clean();

	/*	PDO Driver for MySQL, client library version => 6.0.3-alpha	*/
	$expected = sprintf('Client API version => %s',
		$db->getAttribute(PDO::ATTR_CLIENT_VERSION));

	if (false === stristr($tmp, $expected)) {
		printf("[001] Cannot find MySQL PDO driver line in phpinfo() output\n");
	}

	print "done!";
?>
--EXPECT--
done!
