--TEST--
GH-23016 (NULL in a long column is fetched as a garbage binary string)
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
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$pdo->exec('CREATE TABLE test_gh23016 (data text)');
$pdo->exec('INSERT INTO test_gh23016 VALUES (NULL)');

$row = $pdo->query('SELECT data FROM test_gh23016')->fetch(PDO::FETCH_NUM);
var_dump($row[0]);
?>
--EXPECT--
NULL
