--TEST--
PDO::ATTR_DRIVER_NAME
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

	assert(('' == $db->errorCode()) || ('00000' == $db->errorCode()));

	$name = $db->getAttribute(PDO::ATTR_DRIVER_NAME);
	var_dump($name);

	if (false !== $db->setAttribute(PDO::ATTR_DRIVER_NAME, 'mydriver'))
		printf("[001] Wonderful, I can create new PDO drivers!\n");

	$new_name = $db->getAttribute(PDO::ATTR_DRIVER_NAME);
	if ($name != $new_name)
		printf("[002] Did we change it from '%s' to '%s'?\n", $name, $new_name);

	print "done!";
?>
--EXPECTF--
%unicode|string%(5) "mysql"
done!