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
Pdo\Pgsql::connect() cannot be used for connecting to the "sqlite" driver, either call Pdo\Sqlite::connect() or PDO::connect() instead
