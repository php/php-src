--TEST--
Bug #76449 (SIGSEGV in firebird_handle_doer)
--EXTENSIONS--
pdo_firebird
sockets
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
require_once "payload_server.inc";

$address = run_server(__DIR__ . "/bug_76449.data");

// no need to change the credentials; we're running against a fake server
$dsn = "firebird:dbname=inet://$address/test";
$username = 'SYSDBA';
$password = 'masterkey';

$dbh = new PDO($dsn, $username, $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
var_dump($dbh->exec("INSERT INTO test VALUES ('hihi2', 'xxxxx')"));
?>
--EXPECT--
bool(false)
