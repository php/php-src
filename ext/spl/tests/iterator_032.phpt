--TEST--
SPL: LimitIterator::getPosition()
--FILE--
<?php

$it = new LimitIterator(new ArrayIterator(array(1,2,3,4)), 1, 2);

foreach($it as $k=>$v)
{
    echo "$k=>$v\n";
    var_dump($it->getPosition());
}

try {
    $it->seek(0);
} catch (\ValueError $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}

$it->seek(2);
var_dump($it->current());

try {
    $it->seek(3);
} catch (\ValueError $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}

$it->next();
var_dump($it->valid());

?>
--EXPECT--
1=>2
int(1)
2=>3
int(2)
ValueError: Seek position is out of bounds
int(3)
ValueError: Seek position is out of bounds
bool(false)
