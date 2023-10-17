--TEST--
PDO_sqlite: Test PARAM_BINARY with a BLOB
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

// This is a one pixel PNG of rgba(0, 0, 0, 255)
$binary = base64_decode("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVQIW2NgYGD4DwABBAEAwS2OUAAAAABJRU5ErkJggg==");

$db = new PDO("sqlite::memory:");
$db->exec("CREATE TABLE test_binary(field BLOB)");

$stmt = $db->prepare("INSERT INTO test_binary(field) VALUES (?)");
$stmt->bindParam(1, $binary, PDO::PARAM_BINARY);
$result = $stmt->execute();
var_dump($result);

// We have to bind the result as a PDO binary/SQLite BLOB,
// because just getting the results otherwise will be
// considered a "null" type to SQLite.
$stmt = $db->prepare("SELECT field FROM test_binary");
$result = $stmt->execute();
$binary = "";
$stmt->bindColumn(1, $binary, PDO::PARAM_BINARY);
$result = $stmt->execute();
$result = $stmt->fetch();

var_dump(base64_encode($binary));
var_dump($stmt->getColumnMeta(0)["pdo_type"]);
var_dump($stmt->getColumnMeta(0)["sqlite:decl_type"]);
?>
--EXPECT--
bool(true)
string(96) "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVQIW2NgYGD4DwABBAEAwS2OUAAAAABJRU5ErkJggg=="
int(6)
string(4) "BLOB"
