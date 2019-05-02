<?php

// Test defaults, assumes the following:
// 
// Username: 	SYSDBA
// Password:	phpfi
// Database:	<nothing>
//
// A path to a database must be specified by using PDO_FIREBIRD_TEST_DATABASE

define('PDO_FIREBIRD_TEST_USERNAME', getenv('PDO_FIREBIRD_TEST_USERNAME') ?: 'SYSDBA');
define('PDO_FIREBIRD_TEST_PASSWORD', getenv('PDO_FIREBIRD_TEST_PASSWORD') ?: 'phpfi');
define('PDO_FIREBIRD_TEST_HOSTNAME', getenv('PDO_FIREBIRD_TEST_HOSTNAME') ?: 'localhost');
define('PDO_FIREBIRD_TEST_DATABASE', getenv('PDO_FIREBIRD_TEST_DATABASE') ?: '');

if(!PDO_FIREBIRD_TEST_DATABASE)
{
        die('Error: PDO_FIREBIRD_TEST_DATABASE must be set');
}

if(PDO_FIREBIRD_TEST_HOSTNAME)
{
        $dbh = new PDO('firebird:dbname=' . PDO_FIREBIRD_TEST_HOSTNAME . ':' . PDO_FIREBIRD_TEST_DATABASE, PDO_FIREBIRD_TEST_USERNAME, PDO_FIREBIRD_TEST_PASSWORD) or die;
}
else
{
        $dbh = new PDO('firebird:dbname=' . PDO_FIREBIRD_TEST_DATABASE, PDO_FIREBIRD_TEST_USERNAME, PDO_FIREBIRD_TEST_PASSWORD) or die;
}
?>