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
try
{
	$it->setMaxDepth(4);
	$it->setMaxDepth(-2);
}
catch(Exception $e)
{
	var_dump($e->getMessage());
}
var_dump($it->getMaxDepth());
?>
===DONE===
<?php exit(0); ?>
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
string(33) "Parameter max_depth must be >= -1"
int(4)
===DONE===
