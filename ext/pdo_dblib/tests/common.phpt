--TEST--
DBLIB
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo_dblib')) print 'skip'; 
?>
--REDIRECTTEST--
# magic auto-configuration
# Also update config.inc if you make changes here... 

$config = array(
	'TESTS' => 'ext/pdo/tests'
);

if (false !== getenv('PDO_DBLIB_TEST_DSN')) 
	$config['ENV']['PDOTEST_DSN'] = getenv('PDO_DBLIB_TEST_DSN');
else
	$config['ENV']['PDOTEST_DSN'] = 'dblib:host=localhost dbname=test';

if (false !== getenv('PDO_DBLIB_TEST_USER')) 	
	$config['ENV']['PDOTEST_USER'] = getenv('PDO_DBLIB_TEST_USER');
	
if (false !== getenv('PDO_DBLIB_TEST_PASS')) 	
	$config['ENV']['PDOTEST_PASS'] = getenv('PDO_DBLIB_TEST_PASS');

if (false !== getenv('PDO_DBLIB_TEST_ATTR'))
	$config['ENV']['PDOTEST_ATTR'] = getenv('PDO_DBLIB_TEST_ATTR');
	
return $config;
