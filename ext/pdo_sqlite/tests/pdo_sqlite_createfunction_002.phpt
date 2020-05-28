--TEST--
PDO_sqlite: Testing sqliteCreateFunction() produces warning when
un-callable function passed
--CREDITS--
Chris MacPherson chris@kombine.co.uk
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php

$db = new PDO( 'sqlite::memory:');

try {
    $db->sqliteCreateFunction('bar-alias', 'bar');
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
PDO::sqliteCreateFunction(): Argument #2 must be of type callable, string given
