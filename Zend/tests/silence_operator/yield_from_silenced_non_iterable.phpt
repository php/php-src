--TEST--
Silence yield from not an iterable
--FILE--
<?php

function foo() {
    return 5;
}

function generator() {
    yield 1;
    /* This emits a Error type of throwable which is not suppressed */
    @yield from foo();
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
