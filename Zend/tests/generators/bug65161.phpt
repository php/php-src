--TEST--
Bug #65161: Generator + autoload + syntax error = segfault
--FILE--
<?php

function autoload() {
    foo();
}
spl_autoload_register('autoload');

function testGenerator() {
    new SyntaxError('param');
    yield;
}

foreach (testGenerator() as $i);

?>
--EXPECTF--
Fatal error: Call to undefined function foo() in %s on line %d
