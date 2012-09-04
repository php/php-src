--TEST--
Creating an infinite fibonacci list using a generator
--FILE--
<?php

function fib() {
    list($a, $b) = [1, 1];
    while (true) {
        yield $b;
        list($a, $b) = [$b, $a + $b];
    }
}

foreach (fib() as $n) {
    if ($n > 1000) break;

    var_dump($n);
}

?>
--EXPECT--
int(1)
int(2)
int(3)
int(5)
int(8)
int(13)
int(21)
int(34)
int(55)
int(89)
int(144)
int(233)
int(377)
int(610)
int(987)
