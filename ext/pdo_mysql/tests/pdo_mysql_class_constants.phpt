--TEST--
PDO MySQL specific class constants
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
if (!extension_loaded('mysqli') && !extension_loaded('mysqlnd')) {
	/* Need connection to detect library version */
	require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	MySQLPDOTest::skip();
}
?>
--FILE--
<?php
	require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	$expected = array(
		'MYSQL_ATTR_USE_BUFFERED_QUERY'		=> true,
		'MYSQL_ATTR_LOCAL_INFILE'					=> true,
		'MYSQL_ATTR_DIRECT_QUERY'					=> true,
		'MYSQL_ATTR_FOUND_ROWS'						=> true,
		'MYSQL_ATTR_IGNORE_SPACE'					=> true,
		'MYSQL_ATTR_INIT_COMMAND'					=> true,
		"MYSQL_ATTR_SSL_KEY"						=> true,
		"MYSQL_ATTR_SSL_CERT"						=> true,
		"MYSQL_ATTR_SSL_CA"							=> true,
		"MYSQL_ATTR_SSL_CAPATH"						=> true,
		"MYSQL_ATTR_SSL_CIPHER"						=> true,
		"MYSQL_ATTR_COMPRESS"						=> true,
		"MYSQL_ATTR_MULTI_STATEMENTS"					=> true,
		"MYSQL_ATTR_SSL_VERIFY_SERVER_CERT"				=> true,
	);

	if (!MySQLPDOTest::isPDOMySQLnd()) {
		$expected['MYSQL_ATTR_MAX_BUFFER_SIZE']			= true;
		$expected['MYSQL_ATTR_READ_DEFAULT_FILE']		= true;
		$expected['MYSQL_ATTR_READ_DEFAULT_GROUP']		= true;
	}

	if (extension_loaded('mysqlnd')) {
		$expected['MYSQL_ATTR_SERVER_PUBLIC_KEY']		= true;
	} else if (extension_loaded('mysqli')) {
	    if (mysqli_get_client_version() > 50605) {
			$expected['MYSQL_ATTR_SERVER_PUBLIC_KEY']	= true;
	    }
	} else if (MySQLPDOTest::getClientVersion(MySQLPDOTest::factory()) > 50605) {
		/* XXX the MySQL client library version isn't exposed with any
		constants, the single possibility is to use the PDO::getAttribute().
		This however will fail with no connection. */
		$expected['MYSQL_ATTR_SERVER_PUBLIC_KEY']		= true;
	}

	/*
	TODO

		MYSQLI_OPT_CONNECT_TIMEOUT != PDO::ATTR_TIMEOUT  (integer)
    Sets the timeout value in seconds for communications with the database.
		^  Potential BUG, PDO::ATTR_TIMEOUT is used in pdo_mysql_handle_factory

		MYSQLI_SET_CHARSET_NAME -> DSN/charset=<charset_name>
		^ Undocumented and pitfall for ext/mysqli users

		Assorted mysqlnd settings missing
	*/
	$ref = new ReflectionClass('PDO');
	$constants = $ref->getConstants();
	$values = array();

	foreach ($constants as $name => $value)
		if (substr($name, 0, 11) == 'MYSQL_ATTR_') {
			if (!isset($values[$value]))
				$values[$value] = array($name);
			else
				$values[$value][] = $name;

			if (isset($expected[$name])) {
				unset($expected[$name]);
				unset($constants[$name]);
			}

		} else {
			unset($constants[$name]);
		}

	if (!empty($constants)) {
		printf("[001] Dumping list of unexpected constants\n");
		var_dump($constants);
	}

	if (!empty($expected)) {
		printf("[002] Dumping list of missing constants\n");
		var_dump($expected);
	}

	if (!empty($values)) {
		foreach ($values as $value => $constants) {
			if (count($constants) > 1) {
				printf("[003] Several constants share the same value '%s'\n", $value);
				var_dump($constants);
			}
		}
	}

	print "done!";
--EXPECT--
done!
