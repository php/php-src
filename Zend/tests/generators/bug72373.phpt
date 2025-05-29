--TEST--
Bug #72373: TypeError after Generator function w/declared return type finishes
--FILE--
<?php

function foo() : Generator {
    yield 1;
    yield 2;
    yield 3;
}

foreach (foo() as $bar) {
    echo $bar . "\n";
}

?>
--EXPECT--
1
2
3
