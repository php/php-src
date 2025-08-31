--TEST--
Throw value error if cursor name is too long
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
require 'testdb.inc';

$dbh = getDbConnection();
$query = 'SELECT 1 FROM RDB$DATABASE';
$stmt = $dbh->query($query);

try {
    $stmt->setAttribute(PDO::ATTR_CURSOR_NAME, str_repeat('a', 35));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Cursor name must not be longer than 31 bytes
