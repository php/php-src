--TEST--
SPL: RecursiveIteratorIterator with custom iterator class
--FILE--
<?php

$ar = array(1, 2, array(31, 32, array(331)), 4);

foreach(new RecursiveIteratorIterator(new ArrayObject($ar, 0, "RecursiveArrayIterator")) as $v) echo "$v\n";

$it = new ArrayObject($ar);
var_dump($it->getIteratorClass());

try
{
    foreach(new RecursiveIteratorIterator(new ArrayObject($ar)) as $v) echo "$v\n";
}
catch (InvalidArgumentException $e)
{
    echo $e->getMessage() . "\n";
}

echo "===MANUAL===\n";

$it->setIteratorClass("RecursiveArrayIterator");
var_dump($it->getIteratorClass());
foreach(new RecursiveIteratorIterator($it) as $v) echo "$v\n";


?>
--EXPECT--
1
2
31
32
331
4
string(13) "ArrayIterator"
An instance of RecursiveIterator or IteratorAggregate creating it is required
===MANUAL===
string(22) "RecursiveArrayIterator"
1
2
31
32
331
4
