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
Fatal error: Uncaught Error: Call to undefined function foo() in %s:%d
Stack trace:
#0 %s(%d): autoload('SyntaxError')
#1 %s(%d): testGenerator()
#2 {main}
  thrown in %s on line %d
