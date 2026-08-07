--TEST--
SPL: LimitIterator zero is valid offset
--FILE--
<?php

$array = array('a', 'b', 'c');
$arrayIterator = new ArrayIterator($array);

$limitIterator = new LimitIterator($arrayIterator, 0);
foreach ($limitIterator as $item) {
    echo $item . "\n";
}

try {
    $limitIterator = new LimitIterator($arrayIterator, -1);
} catch (\ValueError $e){
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
a
b
c
ValueError: LimitIterator::__construct(): Argument #2 ($offset) must be greater than or equal to 0
