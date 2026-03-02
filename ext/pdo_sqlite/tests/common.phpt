--TEST--
SQLite
--EXTENSIONS--
pdo_sqlite
--REDIRECTTEST--
return array(
	'ENV' => array(
			'PDOTEST_DSN' => 'sqlite::memory:'
		),
	'TESTS' => __DIR__ . '/ext/pdo/tests'
	);
