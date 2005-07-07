--TEST--
SQLite2
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo') || !extension_loaded('sqlite')) print 'skip'; ?>
--REDIRECTTEST--
return array(
	'ENV' => array(
			'PDOTEST_DSN' => 'sqlite2::memory:'
		),
	'TESTS' => 'ext/pdo/tests'
	);
