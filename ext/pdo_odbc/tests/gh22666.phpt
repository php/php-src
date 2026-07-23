--TEST--
GH-22666 (Heap buffer overflow when an output param value exceeds its maxlen)
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

// An INPUT_OUTPUT parameter declares a maxlen of 4, so the output buffer is 4
// bytes, but the runtime value is longer. The copy into that buffer must be
// bounded to the declared capacity, not overflow it.
$value = str_repeat('A', 64);
$stmt = $pdo->prepare('SELECT ?');
$stmt->bindParam(1, $value, PDO::PARAM_STR | PDO::PARAM_INPUT_OUTPUT, 4);
$stmt->execute();

echo "bounded to maxlen: "; var_dump($value);
?>
--EXPECT--
bounded to maxlen: string(4) "AAAA"
