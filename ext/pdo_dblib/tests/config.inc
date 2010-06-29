<?php

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
	
foreach ($config['ENV'] as $k => $v) {
	putenv("$k=$v");
}
?>
