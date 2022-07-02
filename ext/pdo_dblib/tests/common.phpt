--TEST--
DBLIB
--EXTENSIONS--
pdo_dblib
--REDIRECTTEST--
# magic auto-configuration

return [
	'ENV' => [
		'PDOTEST_DSN'  => getenv('PDO_DBLIB_TEST_DSN')  ?: 'dblib:host=localhost;dbname=test',
		'PDOTEST_USER' => getenv('PDO_DBLIB_TEST_USER') ?: 'php',
		'PDOTEST_PASS' => getenv('PDO_DBLIB_TEST_PASS') ?: 'password',
	],
	'TESTS' => __DIR__ . '/ext/pdo/tests',
];
