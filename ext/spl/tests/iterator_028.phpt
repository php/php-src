--TEST--
SPL: RecursiveIteratorIterator and setMaxDepth()
--FILE--
<?php

$ar = array(1, 2, array(31, 32, array(331, array(3321, array(33221)))), 4);

$it = new RecursiveIteratorIterator(new RecursiveArrayIterator($ar));

echo "===?===\n";
var_dump($it->getMaxDepth());
foreach($it as $v) echo $it->getDepth() . ": $v\n";

echo "===2===\n";
$it->setMaxDepth(2);
var_dump($it->getMaxDepth());
foreach($it as $v) echo $it->getDepth() . ": $v\n";

echo "===X===\n";
$it->setMaxDepth();
var_dump($it->getMaxDepth());
foreach($it as $v) echo $it->getDepth() . ": $v\n";

echo "===3===\n";
$it->setMaxDepth(3);
var_dump($it->getMaxDepth());
foreach($it as $v) echo $it->getDepth() . ": $v\n";

echo "===5===\n";
$it->setMaxDepth(5);
var_dump($it->getMaxDepth());
foreach($it as $v) echo $it->getDepth() . ": $v\n";

echo "===0===\n";
$it->setMaxDepth(0);
var_dump($it->getMaxDepth());
foreach($it as $v) echo $it->getDepth() . ": $v\n";

echo "===-1===\n";
$it->setMaxDepth(-1);
var_dump($it->getMaxDepth());
$it->setMaxDepth(4);
try {
    $it->setMaxDepth(-2);
} catch(\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump($it->getMaxDepth());
?>
--EXPECT--
===?===
bool(false)
0: 1
0: 2
1: 31
1: 32
2: 331
3: 3321
4: 33221
0: 4
===2===
int(2)
0: 1
0: 2
1: 31
1: 32
2: 331
0: 4
===X===
bool(false)
0: 1
0: 2
1: 31
1: 32
2: 331
3: 3321
4: 33221
0: 4
===3===
int(3)
0: 1
0: 2
1: 31
1: 32
2: 331
3: 3321
0: 4
===5===
int(5)
0: 1
0: 2
1: 31
1: 32
2: 331
3: 3321
4: 33221
0: 4
===0===
int(0)
0: 1
0: 2
0: 4
===-1===
bool(false)
RecursiveIteratorIterator::setMaxDepth(): Argument #1 ($maxDepth) must be greater than or equal to -1
int(4)
