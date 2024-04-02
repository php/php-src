--TEST--
Bug #76448 (Stack buffer overflow in firebird_info_cb)
--EXTENSIONS--
pdo_firebird
sockets
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require_once "payload_server.inc";

$address = run_server(__DIR__ . "/bug_76448.data");

// no need to change the credentials; we're running against a falke server
$dsn = "firebird:dbname=inet://$address/test";
$username = 'SYSDBA';
$password = 'masterkey';

$dbh = new PDO($dsn, $username, $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
var_dump($dbh->getAttribute(PDO::ATTR_SERVER_INFO));
?>
--EXPECT--
bool(false)
