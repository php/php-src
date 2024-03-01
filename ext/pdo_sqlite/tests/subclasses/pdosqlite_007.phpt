--TEST--
Calling PDO::connect through the wrong classname
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

class MyPDO extends PDO {}

try {
    MyPDO::connect('sqlite::memory:');
} catch (PDOException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
MyPDO::connect() cannot be called when connecting to the "sqlite" driver, either PdoSqlite::connect() or PDO::connect() must be called instead
