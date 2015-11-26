--TEST--
SPL: FixedArray: Setting size
--FILE--
<?php

$a = new SplFixedArray(0);
$a = new SplFixedArray(3);

$a[0] = 1;

$a->setSize(2);
$a->setSize(3);
$a->setSize(0);

$a = new SplFixedArray(0);
$a->setSize(0);
var_dump($a->getSize());

$a = new SplFixedArray(10);
$a->setSize(10);
var_dump($a->getSize());

$a = new SplFixedArray(1);
$a->setSize(5);
var_dump($a->getSize());

$a = new SplFixedArray(20);
$a->setSize(3);
var_dump($a->getSize());

$a = new SplFixedArray(3);

$a[0] = "test"; 
$a[1] = array(1,2,"blah"); 
$a[2] = 1; 
$a[0] = "test"; 

$a->setSize(0);
var_dump($a->getSize());

print "ok\n";

?>
--EXPECT--
int(0)
int(10)
int(5)
int(3)
int(0)
ok
