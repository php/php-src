<?php

/*
 * Please change $user, $password and $dbase to match your configuration.
 *
 * Set $oracle_on_localhost to TRUE if the Oracle Database is
 * installed on your localhost.
 *
 * Set $test_drcp to TRUE if you want to run the Oracle Database
 * Resident Connection Pooling (DRCP) tests. For these tests to run
 * successfully, you need a server and client which is Oracle 11g or
 * greater, and $dbase should be set to the tnsnames.ora entry
 * corresponding to the POOLED server instance or an Easy Connect
 * string like hostname:port/service_name:POOLED
 */

if (false !== getenv('PHP_OCI8_TEST_DB')) {
	$user		= getenv('PHP_OCI8_TEST_USER');	  // Database username for tests
	$password	= getenv('PHP_OCI8_TEST_PASS');	  // Password for $user
	$dbase		= getenv('PHP_OCI8_TEST_DB');     // Database connection string
	$test_drcp	= getenv('PHP_OCI8_TEST_DRCP');
	if (false !== $test_drcp && 0 == strcasecmp($test_drcp,'TRUE')) {
		$test_drcp = TRUE;
	} else {
		$test_drcp = FALSE;
	}
	$oracle_on_localhost = getenv('PHP_OCI8_TEST_DB_ON_LOCALHOST');
	if (false !== $oracle_on_localhost && 0 == strcasecmp($oracle_on_localhost,'TRUE')) {
		$oracle_on_localhost = TRUE;
	} else {
		$oracle_on_localhost = FALSE;
	}
} else {
	$user						= "system";
	$password					= "system";
	$dbase						= "localhost/XE";
	$oracle_on_localhost		= TRUE;
	$test_drcp					= FALSE;
}

/*
 * Common object names for scripts to use
 */

$table_name = "tb".substr(str_replace(Array(".", "-"), "_", php_uname("n")), 0, 5);
$type_name = strtoupper("tp".substr(str_replace(Array(".", "-"), "_", php_uname("n")), 0, 5));
$schema = '';

?>
