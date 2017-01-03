--TEST--
Postgres
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo_pgsql')) print 'skip'; ?>
--REDIRECTTEST--
# magic auto-configuration
# Also update config.inc if you make changes here...

$config = array(
	'TESTS' => __DIR__ . '/ext/pdo/tests'
);

if (false !== getenv('PDO_PGSQL_TEST_DSN')) {
	# user set them from their shell
	$config['ENV']['PDOTEST_DSN'] = getenv('PDO_PGSQL_TEST_DSN');
	if (false !== getenv('PDO_PGSQL_TEST_ATTR')) {
		$config['ENV']['PDOTEST_ATTR'] = getenv('PDO_PGSQL_TEST_ATTR');
	}
} else {
	$config['ENV']['PDOTEST_DSN'] = 'pgsql:host=localhost port=5432 dbname=test user= password=';
}

return $config;
