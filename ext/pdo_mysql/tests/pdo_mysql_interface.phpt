--TEST--
MySQL PDO class interface
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::getDriver();
if (false == MySQLPDOTest::detect_transactional_mysql_engine($db))
	die("skip Transactional engine not found");
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();

	$expected = array(
		'__construct'					=> true,
		'prepare' 						=> true,
		'beginTransaction'		=> true,
		'commit'							=> true,
		'rollBack'						=> true,
		'setAttribute'				=> true,
		'exec'								=> true,
		'query'								=> true,
		'lastInsertId'				=> true,
		'errorCode'						=> true,
		'errorInfo'						=> true,
		'getAttribute'				=> true,
		'quote'								=> true,
		'__wakeup'						=> true,
		'__sleep'							=> true,
		'getAvailableDrivers'	=> true,
	);
	$classname = get_class($db);

	$methods = get_class_methods($classname);
	foreach ($methods as $k => $method) {
		if (isset($expected[$method])) {
			unset($expected[$method]);
			unset($methods[$k]);
		}
		if ($method == $classname) {
			unset($expected['__construct']);
			unset($methods[$k]);
		}
	}
	if (!empty($expected)) {
		printf("Dumping missing class methods\n");
		var_dump($expected);
	}
	if (!empty($methods)) {
		printf("Found more methods than expected, dumping list\n");
		var_dump($methods);
	}

	print "done!";
--EXPECT--
done!