--TEST--
FB payload server satisfies connection attempt
--EXTENSIONS--
pdo_firebird
sockets
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require_once "payload_server.inc";

$address = run_server(__DIR__ . "/payload_test.data");

// no need to change the credentials; we're running against a fake server
$dsn = "firebird:dbname=inet://$address/test";
$username = 'SYSDBA';
$password = 'masterkey';

new PDO($dsn, $username, $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
?>
--EXPECT--
