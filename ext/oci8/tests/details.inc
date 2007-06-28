<?php

/*
 * Please change $user, $password and $dbase to match your configuration.
 * Set $oracle_on_localhost to TRUE if the Oracle Database is installed on your localhost.
 */

if (false !== getenv('PHP_OCI8_TEST_DB')) {
	$user		= getenv('PHP_OCI8_TEST_USER');	  // Database username for tests
	$password	= getenv('PHP_OCI8_TEST_PASS');	  // Password for $user
	$dbase		= getenv('PHP_OCI8_TEST_DB');     // Database connection string
	$oracle_on_localhost = getenv('PHP_OCI8_TEST_DB_ON_LOCALHOST');
	if (false !== $oracle_on_localhost && 0 == strcasecmp($oracle_on_localhost,'TRUE')) {
		$oracle_on_localhost = TRUE;
	} else {
		$oracle_on_localhost = FALSE;
	}
} else {
	$user						= "system";
	$password					= "system";
	$dbase						= "oracle";
	$oracle_on_localhost		= FALSE;
}

?>
