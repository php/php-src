--TEST--
PDO_sqlite: Testing sqliteCreateFunction() produces warning when
un-callable function passed
--CREDITS--
Chris MacPherson chris@kombine.co.uk
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO( 'sqlite::memory:');

try {
    $db->sqliteCreateFunction('bar-alias', 'bar');
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
Deprecated: Method PDO::sqliteCreateFunction() is deprecated since 8.5, use Pdo\Sqlite::createFunction() instead in %s on line %d
PDO::sqliteCreateFunction(): Argument #2 ($callback) must be a valid callback, function "bar" not found or invalid function name
