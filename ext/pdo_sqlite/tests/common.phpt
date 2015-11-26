--TEST--
SQLite
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo_sqlite')) print 'skip'; ?>
--REDIRECTTEST--
return array(
	'ENV' => array(
			'PDOTEST_DSN' => 'sqlite::memory:'
		),
	'TESTS' => 'ext/pdo/tests'
	);
