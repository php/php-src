--TEST--
pdo_odbc: a bound parameter does not leave a stale ODBC binding on re-execute
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
$v = 'bound';
$stmt->bindParam(1, $v, PDO::PARAM_STR | PDO::PARAM_INPUT_OUTPUT, 256);
$stmt->execute(['from_array']);

var_dump($stmt->fetch(PDO::FETCH_ASSOC)['v']);
?>
--EXPECT--
string(10) "from_array"
