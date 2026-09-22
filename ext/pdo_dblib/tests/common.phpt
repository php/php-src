--TEST--
DBLIB
--EXTENSIONS--
pdo_dblib
--REDIRECTTEST--
# magic auto-configuration

$config = [
	'ENV' => [
		'PDOTEST_DSN'  => getenv('PDO_DBLIB_TEST_DSN')  ?: 'dblib:host=localhost;dbname=test',
		'PDOTEST_USER' => getenv('PDO_DBLIB_TEST_USER') ?: 'php',
		'PDOTEST_PASS' => getenv('PDO_DBLIB_TEST_PASS') ?: 'password',
	],
	'TESTS' => __DIR__ . '/ext/pdo/tests',
];

if (getenv('PDO_DBLIB_TEST_DSN') === false) {
	$config['ENV']['PDOTEST_SKIP_ATTR'] = serialize([
		Pdo\Dblib::ATTR_CONNECTION_TIMEOUT => 1,
	]);
}

return $config;
