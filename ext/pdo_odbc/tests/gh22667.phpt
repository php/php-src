--TEST--
GH-22667 (Heap buffer over-read when a column value exceeds the bound buffer)
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

// The SQLite3 driver reports a 255 byte display size for a computed column, so
// the short-bound buffer holds at most 255 bytes while the value is far longer.
// A conforming driver truncates into the buffer but reports the full length; the
// returned string must stay within the buffer, not over-read past it.
$stmt = $pdo->query("SELECT printf('%.*c', 4096, 'A') AS data");
$row = $stmt->fetch(PDO::FETCH_ASSOC);
$s = $row['data'];

echo "clamped to buffer: "; var_dump(strlen($s) < 4096);
echo "only value bytes:  "; var_dump(strlen($s) === substr_count($s, 'A'));
?>
--EXPECT--
clamped to buffer: bool(true)
only value bytes:  bool(true)
