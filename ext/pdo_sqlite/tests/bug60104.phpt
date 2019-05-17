--TEST--
Bug #60104 (Segmentation Fault in pdo_sqlite when using sqliteCreateFunction())
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
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
--EXPECT--
done
