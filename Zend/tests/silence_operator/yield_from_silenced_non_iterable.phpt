--TEST--
Silence yield from not an iterable
--FILE--
<?php

function foo() {
    return 5;
}

function generator() {
    yield 1;
    @yield from foo();
    yield 2;
}

foreach (generator() as $val) {
    var_dump($val);
}

echo "Done\n";
?>
--EXPECT--
int(1)
int(2)
Done
