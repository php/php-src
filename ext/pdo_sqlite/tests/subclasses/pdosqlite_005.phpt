--TEST--
Calling PDO::connect through the wrong classname
--EXTENSIONS--
pdo_pgsql
pdo_sqlite
--FILE--
<?php

try {
    PdoPgSql::connect('sqlite::memory:');
} catch (PDOException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
PdoPgsql::connect() cannot be called when connecting to the "sqlite" driver, either PdoSqlite::connect() or PDO::connect() must be called instead
