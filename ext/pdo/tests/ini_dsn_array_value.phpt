--TEST--
PDO: array-valued pdo.dsn.* INI entry is not used as a DSN alias
--EXTENSIONS--
pdo
--INI--
pdo.dsn.array_value[]=sqlite::memory:
--FILE--
<?php
try {
    new PDO('array_value');
} catch (PDOException $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
PDOException: PDO::__construct(): Argument #1 ($dsn) must be a valid data source name
