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

try
{
    $it->seek(0);
}
catch(OutOfBoundsException $e)
{
    echo $e->getMessage() . "\n";
}

$it->seek(2);
var_dump($it->current());

try
{
    $it->seek(3);
}
catch(OutOfBoundsException $e)
{
    echo $e->getMessage() . "\n";
}

$it->next();
var_dump($it->valid());

?>
--EXPECT--
1=>2
int(1)
2=>3
int(2)
Cannot seek to 0 which is below the offset 1
int(3)
Cannot seek to 3 which is behind offset 1 plus count 2
bool(false)
