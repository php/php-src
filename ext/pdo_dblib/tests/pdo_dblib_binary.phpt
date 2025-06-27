--TEST--
PDO_DBLIB: Ensure binary bound parameter is a binary literal
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

// This is a one pixel PNG of rgba(0, 0, 0, 255)
$binary = base64_decode("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVQIW2NgYGD4DwABBAEAwS2OUAAAAABJRU5ErkJggg==");
$query =
	"SELECT ?";
//	"create table #php_pdo(value varbinary(max));" .
//	"insert into #php_pdo values ?;" .
//	"drop table #php_pdo;";

$db = getDbConnection();

$stmt = $db->prepare($query);
// PARAM_LOB gets converted to a binary literal instead of a string literal
$stmt->bindParam(1, $binary, PDO::PARAM_BINARY);
$result = $stmt->execute();

// Verify we sent the binary literal over the wire
var_dump($stmt->debugDumpParams());

// Verify that we get the same PNG back over the wire
$rows = $stmt->fetchAll();
var_dump(base64_encode($rows[0][0]));

?>
--EXPECT--
SQL: [8] SELECT ?
Sent SQL: [149] SELECT 0x89504E470D0A1A0A0000000D49484452000000010000000108060000001F15C4890000000D49444154085B63606060F80F0001040100C12D8E500000000049454E44AE426082
Params:  1
Key: Position #0:
paramno=0
name=[0] ""
is_param=1
param_type=6
NULL
string(96) "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVQIW2NgYGD4DwABBAEAwS2OUAAAAABJRU5ErkJggg=="
