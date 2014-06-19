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

$db->sqliteCreateFunction('bar-alias', 'bar');

?>
--EXPECTF--
Warning: PDO::sqliteCreateFunction(): function 'bar' is not callable in %s on line %d
