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
MyPDO::connect() cannot be used for connecting to the "sqlite" driver, either call Pdo\Sqlite::connect() or PDO::connect() instead
