<?php

function get_tds_version() {
	global $dsn;

	$dsn_parts = explode(':', $dsn, 2);
	if ($dsn_parts[0] == 'dblib') { // uri is an option, which we'll ignore
		foreach (explode(';', $dsn_parts[1]) as $arg) {
			$arg = explode('=', $arg);
			if ($arg[0] == 'version') {
				return $arg[1];
			}
		}
	}

	return null;
}

if (false !== getenv('PDO_DBLIB_TEST_DSN')) 
	$dsn = getenv('PDO_DBLIB_TEST_DSN');
else
	$dsn = 'dblib:host=localhost;dbname=test';

if (false !== getenv('PDO_DBLIB_TEST_USER')) 	
	$user = getenv('PDO_DBLIB_TEST_USER');
else
	$user = 'php';
	
	
if (false !== getenv('PDO_DBLIB_TEST_PASS')) 	
	$pass = getenv('PDO_DBLIB_TEST_PASS');
else
	$pass = 'password';

try {
	$db = new PDO($dsn, $user, $pass);
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);
} catch (PDOException $e) {
	die('skip ' . $e->getMessage());
}
?>
