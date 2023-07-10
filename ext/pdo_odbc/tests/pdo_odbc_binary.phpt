--TEST--
PDO_odbc: Test PARAM_BINARY
--EXTENSIONS--
pdo_odbc
--FILE--
<?php

// This is a one pixel PNG of rgba(0, 0, 0, 255)
$binary = base64_decode("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVQIW2NgYGD4DwABBAEAwS2OUAAAAABJRU5ErkJggg==");

require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
// XXX: Is this the right type to use?
$db->exec("CREATE TABLE test_binary(field VARBINARY(256))");

$stmt = $db->prepare("INSERT INTO test_binary(field) VALUES (?)");
$stmt->bindParam(1, $binary, PDO::PARAM_BINARY);
$result = $stmt->execute();
var_dump($result);

$stmt = $db->prepare("SELECT field FROM test_binary");
$result = $stmt->execute();
$binary = "";
$stmt->bindColumn(1, $binary, PDO::PARAM_BINARY);
$result = $stmt->execute();
$result = $stmt->fetch();

var_dump(base64_encode($binary));
var_dump($stmt->getColumnMeta(0)["pdo_type"]);
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->exec("DROP TABLE test_binary");
?>
--EXPECT--
bool(true)
string(96) "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVQIW2NgYGD4DwABBAEAwS2OUAAAAABJRU5ErkJggg=="
int(6)
