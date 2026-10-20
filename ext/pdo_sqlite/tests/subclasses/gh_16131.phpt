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
    echo $e::class, ': ', $e->getMessage(), "\n";
}

class MyPgsql extends Pdo\Pgsql
{
}

try {
    new MyPgsql('sqlite::memory:');
} catch (PDOException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
PDOException: Pdo\Pgsql::__construct() cannot be used for connecting to the "sqlite" driver, either call Pdo\Sqlite::__construct() or PDO::__construct() instead
PDOException: MyPgsql::__construct() cannot be used for connecting to the "sqlite" driver, either call Pdo\Sqlite::__construct() or PDO::__construct() instead
