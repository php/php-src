--TEST--
GHSA-w476-322c-wpvm: SQL injection in pdo_firebird via NUL bytes in quoted strings
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

$dbh->exec('CREATE TABLE ghsa_w476_322c_wpvm (name VARCHAR(255))');

$param = $dbh->quote("\0");
$param2 = $dbh->quote('or 1=1--');
var_export($param);
echo("\n");

echo "prepare: ";
$stmt = $dbh->prepare("SELECT * FROM ghsa_w476_322c_wpvm WHERE name = {$param} AND name = {$param2}");
$stmt->execute();
echo json_encode($stmt->fetchAll(PDO::FETCH_ASSOC)) . "\n";

echo "query:   ";
$stmt = $dbh->query("SELECT * FROM ghsa_w476_322c_wpvm WHERE name = {$param} AND name = {$param2}");
echo json_encode($stmt->fetchAll(PDO::FETCH_ASSOC)) . "\n";

echo "exec:    ";
$affectedRows = $dbh->exec("UPDATE ghsa_w476_322c_wpvm SET name = 'updated' WHERE name = {$param} AND name = {$param2}");
echo $affectedRows . "\n";
?>
--CLEAN--
<?php
require 'testdb.inc';
$dbh->exec("DROP TABLE ghsa_w476_322c_wpvm");
?>
--EXPECT--
'\'' . "\0" . '\''
prepare: []
query:   []
exec:    0
