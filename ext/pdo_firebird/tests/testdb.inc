<?php

// Test defaults, assumes the following:
//
// Username: 	SYSDBA
// Password:	phpfi
// Database:	<nothing>
//
// A DSN must be specified by using PDO_FIREBIRD_TEST_DSN

define('PDO_FIREBIRD_TEST_USER', getenv('PDO_FIREBIRD_TEST_USER') ?: 'SYSDBA');
define('PDO_FIREBIRD_TEST_PASS', getenv('PDO_FIREBIRD_TEST_PASS') ?: 'phpfi');
define('PDO_FIREBIRD_TEST_DSN', getenv('PDO_FIREBIRD_TEST_DSN') ?: '');

if(!PDO_FIREBIRD_TEST_DSN)
{
	die('Error: PDO_FIREBIRD_TEST_DSN must be set');
}

function getDbConnection($class = PDO::class): PDO {
	return new $class(PDO_FIREBIRD_TEST_DSN, PDO_FIREBIRD_TEST_USER, PDO_FIREBIRD_TEST_PASS);
}

function connectToDb(array $params = []): Pdo\Firebird {
	return Pdo\Firebird::connect(PDO_FIREBIRD_TEST_DSN, PDO_FIREBIRD_TEST_USER, PDO_FIREBIRD_TEST_PASS, $params);
}

function checkMinServerVersion(string $version): void
{
	$dbh = connectToDb();
	$stmt = $dbh->query("SELECT RDB\$GET_CONTEXT('SYSTEM', 'ENGINE_VERSION') AS VERSION FROM RDB\$DATABASE");
	$data = $stmt->fetch(\PDO::FETCH_ASSOC);
	if (!$data || !array_key_exists('VERSION', $data) || version_compare($data['VERSION'], $version) < 0) {
		die("skip Firebird Server version must be greater than or equal to $version");
	}
}

?>
