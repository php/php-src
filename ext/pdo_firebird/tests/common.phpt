--TEST--
FIREBIRD
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php # vim:ft=php
?>
--REDIRECTTEST--
# magic auto-configuration

$config = array(
    // A bug in firebird causes a memory leak when calling `isc_attach_database()`. See https://github.com/FirebirdSQL/firebird/issues/7849
	'ENV' => ['LSAN_OPTIONS' => 'detect_leaks=0'],
	'TESTS' => 'ext/pdo/tests'
);
	

if (false !== getenv('PDO_FIREBIRD_TEST_DSN')) {
	# user set them from their shell
	$config['ENV']['PDOTEST_DSN'] = getenv('PDO_FIREBIRD_TEST_DSN');
	$config['ENV']['PDOTEST_USER'] = getenv('PDO_FIREBIRD_TEST_USER');
	$config['ENV']['PDOTEST_PASS'] = getenv('PDO_FIREBIRD_TEST_PASS');
	if (false !== getenv('PDO_FIREBIRD_TEST_ATTR')) {
		$config['ENV']['PDOTEST_ATTR'] = getenv('PDO_FIREBIRD_TEST_ATTR');
	}
} else {
	$config['ENV']['PDOTEST_DSN'] = 'firebird:dbname=/opt/firebird/test.gdb';
	$config['ENV']['PDOTEST_USER'] = 'SYSDBA';
	$config['ENV']['PDOTEST_PASS'] = 'password';
}
return $config;
