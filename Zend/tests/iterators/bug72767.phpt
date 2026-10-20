--TEST--
Bug #72767: PHP Segfaults when trying to expand an infinite operator
--FILE--
<?php

function test() {}
$iterator = new LimitIterator(
    new InfiniteIterator(new ArrayIterator([42])),
    0, 17000
);
test(...$iterator);

?>
===DONE===
--EXPECT--
===DONE===
