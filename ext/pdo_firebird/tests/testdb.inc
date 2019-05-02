<?php

// Test defaults, assumes the following:
// 
// Username: 	SYSDBA
// Password:	phpfi
// Database:	<path/to/ext/pdo_firebird/tests/>php.fdb
//
// (Assumes FireBird flavor over InterBase by default)

define('PDO_FIREBIRD_USERNAME', getenv('PDO_FIREBIRD_USERNAME') ?: 'SYSDBA');
define('PDO_FIREBIRD_PASSWORD', getenv('PDO_FIREBIRD_PASSWORD') ?: 'phpfi');
define('PDO_FIREBIRD_HOSTNAME', getenv('PDO_FIREBIRD_HOSTNAME') ?: 'localhost');
define('PDO_FIREBIRD_DATABASE', getenv('PDO_FIREBIRD_DATABASE') ?: __DIR__ . DIRECTORY_SEPARATOR . 'php.fdb');

$dbh = new PDO('firebird:dbname=' . PDO_FIREBIRD_HOSTNAME . ':' . PDO_FIREBIRD_DATABASE, PDO_FIREBIRD_USERNAME, PDO_FIREBIRD_PASSWORD) or die;
?>