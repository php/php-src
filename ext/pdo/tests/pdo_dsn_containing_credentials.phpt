--TEST--
PDO Common: Pass credentials in dsn instead of constructor params
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');

$driver = substr(getenv('PDOTEST_DSN'), 0, strpos(getenv('PDOTEST_DSN'), ':'));
if (!in_array($driver, array('mssql','sybase','dblib','firebird','mysql','oci')))
	die('skip not supported');

require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
	require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

	class PDOTestDiscardCredentialParams extends PDOTest {
		public function __construct($dsn, $user, $pass, $attr) {
			return parent::__construct($dsn, null, null, $attr);
		}
	}

	$link = PDOTestDiscardCredentialParams::factory('PDO', false, null, ['user' => getenv('PDOTEST_USER'), 'password' => getenv('PDOTEST_PASS')]);
	echo "using credentials in dsn: done\n";

	// test b/c - credentials in DSN are ignored when user/pass passed as separate params
	$link = PDOTest::factory('PDO', false, null, ['user' => 'incorrect', 'password' => 'ignored']);
	echo "ignoring credentials in dsn: done\n";
?>
--EXPECTF--
using credentials in dsn: done
ignoring credentials in dsn: done
