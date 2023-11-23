--TEST--
SQLite
--EXTENSIONS--
pdo_sqlite
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) {
    // finished in 2.22s in a best-of-3 on an idle Intel Xeon X5670 on PHP 8.3.0-dev
    die("skip slow test");
}
?>
--REDIRECTTEST--
return array(
	'ENV' => array(
			'PDOTEST_DSN' => 'sqlite::memory:'
		),
	'TESTS' => __DIR__ . '/ext/pdo/tests'
	);
