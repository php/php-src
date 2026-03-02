--TEST--
GH-17383 (PDOException has wrong code and message since PHP 8.4)
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php

require("testdb.inc");
unset($dbh);

foreach ([
    ['firebird:dbname=invalid_host:db', PDO_FIREBIRD_TEST_USER, PDO_FIREBIRD_TEST_PASS],
    [PDO_FIREBIRD_TEST_DSN, 'invalid_user', PDO_FIREBIRD_TEST_PASS],
    [PDO_FIREBIRD_TEST_DSN, PDO_FIREBIRD_TEST_USER, 'invalid_pass'],
] as [$dsn, $user, $pass]) {
    try {
        $dbh = new PDO($dsn, $user, $pass);
    } catch (PDOException $e) {
		echo 'PDOException code: ' . $e->getCode() . "\n";
		echo 'PDOException message: ' . $e->getMessage() . "\n";
        echo "\n";
    }
}
?>
--EXPECT--
PDOException code: 335544721
PDOException message: SQLSTATE[HY000] [335544721] Unable to complete network request to host "invalid_host".

PDOException code: 335544472
PDOException message: SQLSTATE[HY000] [335544472] Your user name and password are not defined. Ask your database administrator to set up a Firebird login.

PDOException code: 335544472
PDOException message: SQLSTATE[HY000] [335544472] Your user name and password are not defined. Ask your database administrator to set up a Firebird login.
