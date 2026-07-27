--TEST--
GH-22665 (OOB write in pdo_odbc_error when the driver reports a long message)
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
$pdo = new PDO(PDO_ODBC_SQLITE_DSN, null, null, [
    PDO::ATTR_ERRMODE => PDO::ERRMODE_SILENT,
]);

// A failing query with a long identifier makes the driver report a diagnostic
// message length >= the fixed last_err_msg buffer. The terminator write must
// stay inside the buffer.
$pdo->query('SELECT * FROM "' . str_repeat('A', 4096) . '"');
$info = $pdo->errorInfo();

echo "sqlstate: ", $info[0], "\n";
echo "done\n";
?>
--EXPECT--
sqlstate: HY000
done
