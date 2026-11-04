--TEST--
GH-25006 (PDO_Firebird::getColumnMeta() reads out of bounds for an out-of-range column index)
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php
require 'skipif.inc';
?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require 'testdb.inc';

$dbh = getDbConnection();
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$stmt = $dbh->prepare('SELECT 1 AS one, 2 AS two FROM RDB$DATABASE');

echo "before execute: ";
try {
    var_dump($stmt->getColumnMeta(2));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$stmt->execute();

echo "column 0: ";
$meta = $stmt->getColumnMeta(0);
echo $meta['name'], ' ', $meta['pdo_type'], "\n";

echo "column 1: ";
$meta = $stmt->getColumnMeta(1);
echo $meta['name'], ' ', $meta['pdo_type'], "\n";

foreach ([2, 1000000] as $col) {
    echo "column $col: ";
    try {
        var_dump($stmt->getColumnMeta($col));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

echo "no output columns: ";
$empty = $dbh->prepare('EXECUTE BLOCK AS BEGIN END');
echo $empty->columnCount(), ' ';
try {
    var_dump($empty->getColumnMeta(0));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "negative: ";
try {
    var_dump($stmt->getColumnMeta(-1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "done\n";
?>
--EXPECT--
before execute: PDOException: SQLSTATE[07009]: Invalid descriptor index: invalid column index
column 0: ONE 1
column 1: TWO 1
column 2: PDOException: SQLSTATE[07009]: Invalid descriptor index: invalid column index
column 1000000: PDOException: SQLSTATE[07009]: Invalid descriptor index: invalid column index
no output columns: 0 PDOException: SQLSTATE[07009]: Invalid descriptor index: invalid column index
negative: ValueError: PDOStatement::getColumnMeta(): Argument #1 ($column) must be greater than or equal to 0
done
