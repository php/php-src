--TEST--
Calling PDO::connect through the wrong classname
--EXTENSIONS--
pdo_pgsql
pdo_sqlite
--FILE--
<?php

try {
    Pdo\Pgsql::connect('sqlite::memory:');
} catch (PDOException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Pdo\Pgsql::connect() cannot be called when connecting to the "sqlite" driver, either Pdo\Sqlite::connect() or PDO::connect() must be called instead
