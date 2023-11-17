--TEST--
Bug #76449 (SIGSEGV in firebird_handle_doer)
--EXTENSIONS--
pdo_firebird
sockets
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require_once "payload_server.inc";

$address = run_server(__DIR__ . "/bug_76449.data");

// no need to change the credentials; we're running against a fake server
$dsn = "firebird:dbname=inet://$address/test76449";
$username = 'SYSDBA';
$password = 'masterkey';

$dbh = new PDO($dsn, $username, $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
var_dump($dbh->exec("INSERT INTO test76449 VALUES ('hihi2', 'xxxxx')"));
?>
--EXPECT--
bool(false)
