--TEST--
Throw value error if cursor name is too long
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require 'skipif.inc'; ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require 'testdb.inc';

$dbh = getDbConnection();
$query = 'SELECT 1';
$stmt = $dbh->query($query);

try {
    $stmt->setAttribute(PDO::ATTR_CURSOR_NAME, str_repeat('a', 35));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Cursor name must not be longer than 31 bytes
