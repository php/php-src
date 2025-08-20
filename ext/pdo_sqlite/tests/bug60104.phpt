--TEST--
Bug #60104 (Segmentation Fault in pdo_sqlite when using sqliteCreateFunction())
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
function setUp()
{
    $handler = new PDO( "sqlite::memory:" );
    $handler->sqliteCreateFunction( "md5", "md5", 1 );
    unset( $handler );
}

setUp();
setUp();
echo "done";
?>
--EXPECTF--
Deprecated: Method PDO::sqliteCreateFunction() is deprecated since 8.5, use Pdo\Sqlite::createFunction() instead in %s on line %d

Deprecated: Method PDO::sqliteCreateFunction() is deprecated since 8.5, use Pdo\Sqlite::createFunction() instead in %s on line %d
done
