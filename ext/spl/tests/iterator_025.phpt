--TEST--
SPL: RecursiveIteratorIterator and begin/endIteration()
--FILE--
<?php

class MyRecursiveIteratorIterator extends RecursiveIteratorIterator
{
    function beginIteration(): void
    {
        echo __METHOD__ . "()\n";
    }

    function endIteration(): void
    {
        echo __METHOD__ . "()\n";
    }
}

$ar = array(1, 2, array(31, 32, array(331)), 4);

$it = new MyRecursiveIteratorIterator(new ArrayObject($ar, 0, "RecursiveArrayIterator"));

foreach($it as $v) echo "$v\n";

echo "===MORE===\n";

foreach($it as $v) echo "$v\n";

echo "===MORE===\n";

$it->rewind();
foreach($it as $v) echo "$v\n";
var_dump($it->valid());

echo "===MANUAL===\n";

$it->rewind();
while($it->valid())
{
    echo $it->current() . "\n";
    $it->next();
    break;
}
$it->rewind();
while($it->valid())
{
    echo $it->current() . "\n";
    $it->next();
}

?>
--EXPECT--
MyRecursiveIteratorIterator::beginIteration()
1
2
31
32
331
4
MyRecursiveIteratorIterator::endIteration()
===MORE===
MyRecursiveIteratorIterator::beginIteration()
1
2
31
32
331
4
MyRecursiveIteratorIterator::endIteration()
===MORE===
MyRecursiveIteratorIterator::beginIteration()
1
2
31
32
331
4
MyRecursiveIteratorIterator::endIteration()
bool(false)
===MANUAL===
MyRecursiveIteratorIterator::beginIteration()
1
1
2
31
32
331
4
MyRecursiveIteratorIterator::endIteration()
