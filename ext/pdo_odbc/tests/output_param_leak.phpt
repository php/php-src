--TEST--
pdo_odbc: bound output parameter buffer is released (no leak)
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
try {
    $pdo = new PDO(PDO_ODBC_SQLITE_DSN);
} catch (PDOException $e) {
    die("skip requires the SQLite3 ODBC driver");
}
?>
--FILE--
<?php
require __DIR__ . '/config.inc';
$pdo = new PDO(PDO_ODBC_SQLITE_DSN);
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$stmt = $pdo->prepare('SELECT ? AS v');
$var = 'seed';
$stmt->bindParam(1, $var, PDO::PARAM_STR | PDO::PARAM_INPUT_OUTPUT, 256);
$stmt->execute();
$row = $stmt->fetch(PDO::FETCH_ASSOC);

var_dump($row['v']);
?>
--EXPECT--
string(4) "seed"
