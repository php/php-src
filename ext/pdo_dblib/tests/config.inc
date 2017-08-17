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

// bug #72969 reflects a bug with FreeTDS, not with pdo_dblib
// this function will version detect so the relevant tests can XFAILIF
function driver_supports_batch_statements_without_select($db) {
	$version = $db->getAttribute(PDO::DBLIB_ATTR_VERSION);

	// assume driver doesn't have this bug if not using FreeTDS
	if (!strstartswith($version, 'freetds ')) {
		return true;
	}

	// hasn't made it to numbered release yet
	if (!strstartswith($version, 'freetds v1.1.dev.')) {
		return false;
	}

	// fc820490336c50d5c175d2a15327383256add4c9 was committed on the 5th
	return intval(substr($version, -8)) >= 20161206;
}

function strstartswith($haystack, $needle) {
	return $needle === "" || strrpos($haystack, $needle, -strlen($haystack)) !== false;
}

if (false !== getenv('PDO_DBLIB_TEST_DSN')) {
	$dsn = getenv('PDO_DBLIB_TEST_DSN');
} else {
	$dsn = 'dblib:host=localhost;dbname=test';
}

if (false !== getenv('PDO_DBLIB_TEST_USER')) {
	$user = getenv('PDO_DBLIB_TEST_USER');
} else {
	$user = 'php';
}

if (false !== getenv('PDO_DBLIB_TEST_PASS')) {
	$pass = getenv('PDO_DBLIB_TEST_PASS');
} else {
	$pass = 'password';
}

try {
	$db = new PDO($dsn, $user, $pass);
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);
} catch (PDOException $e) {
	die('skip ' . $e->getMessage());
}

?>
