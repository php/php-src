--TEST--
PDO_MYSQL: Pass credentials in dsn instead of constructor params
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	class MySQLPDOTestDiscardCredentialParams extends MySQLPDOTest {
		public function __construct($dsn, $user, $pass, $attr) {
			return parent::__construct($dsn, null, null, $attr);
		}
	}

	$link = MySQLPDOTestDiscardCredentialParams::factory('PDO', false, null, ['user' => PDO_MYSQL_TEST_USER, 'password' => PDO_MYSQL_TEST_PASS]);
	echo "using credentials in dsn: done\n";

	// test b/c - credentials in DSN are ignored when user/pass passed as separate params
	$link = MySQLPDOTest::factory('PDO', false, null, ['user' => 'incorrect', 'password' => 'ignored']);
	echo "ignoring credentials in dsn: done\n";
?>
--EXPECTF--
using credentials in dsn: done
ignoring credentials in dsn: done
