--TEST--
LimitIterator: do not seek if not needed
--FILE--
<?php

$it = new ArrayIterator(array());

$lit = new LimitIterator($it, 0, 5);

foreach ($lit as $v) {
    echo $v;
}
?>
===DONE===
--EXPECT--
===DONE===
