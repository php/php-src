--TEST--
SQLite
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip'; ?>
--REDIRECTTEST--
return array(
	'ENV' => array(
			'PDOTEST_DSN' => 'sqlite::memory:'
		),
	'TESTS' => __DIR__ . '/ext/pdo/tests'
	);
