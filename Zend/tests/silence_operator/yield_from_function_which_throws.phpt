--TEST--
yield from @expr throws
--FILE--
<?php

function foo(): iterable {
    throw new Exception();
    return [1];
}

function generator() {
    yield 1;
    yield from @foo();
    yield 2;
}

foreach (generator() as $val) {
    var_dump($val);
}

echo "Done\n";
?>
--EXPECTF--
int(1)

Fatal error: Uncaught Error: Can use "yield from" only with arrays and Traversables in %s:%d
Stack trace:
#0 %s(%d): generator()
#1 {main}
  thrown in %s on line %d
