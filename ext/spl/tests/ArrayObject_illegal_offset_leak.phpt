--TEST--
Assignments to illegal ArrayObject offsets shouldn't leak
--FILE--
<?php

$ao = new ArrayObject([1, 2, 3]);
try {
    $ao[[]] = new stdClass;
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Offsets must be int|string, array given
