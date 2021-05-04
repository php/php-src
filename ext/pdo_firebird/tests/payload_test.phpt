--TEST--
FB payload server satisfies connection attempt
--SKIPIF--
<?php
if (!extension_loaded('sockets')) die("skip sockets extension not available");
?>
--FILE--
<?php
require_once "payload_server.inc";

$address = run_server(__DIR__ . "/payload_test.data");

// no need to change the credentials; we're running against a falke server
$dsn = "firebird:dbname=inet://$address/test";
$username = 'SYSDBA';
$password = 'masterkey';

new PDO($dsn, $username, $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
?>
--EXPECT--
