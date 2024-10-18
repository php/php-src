--TEST--
Test calling a PDO sub-class constructor with a different DSN
--EXTENSIONS--
pdo_pgsql
pdo_sqlite
--FILE--
<?php

try {
    new Pdo\Pgsql('sqlite::memory:');
} catch (PDOException $e) {
    echo $e->getMessage() . "\n";
}

class Foo extends Pdo\Pgsql
{
}

try {
    new Foo('sqlite::memory:');
} catch (PDOException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Pdo\Pgsql::__construct() cannot be called when connecting to the "sqlite" driver, either call Pdo\Sqlite::__construct() or PDO::__construct() instead
Pdo\Pgsql::__construct() cannot be called when connecting to the "sqlite" driver, either call Pdo\Sqlite::__construct() or PDO::__construct() instead
