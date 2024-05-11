--TEST--
Calling PDO::connect through the wrong classname
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

class MyPdoSqlite extends PdoSqlite {}

$db = MyPdoSqlite::connect('sqlite::memory:');
if (!$db instanceof MyPdoSqlite) {
    echo "Wrong class type. Should be MyPdoSqlite but is " . get_class($db) . "\n";
}
echo "OK";
?>
--EXPECT--
OK
