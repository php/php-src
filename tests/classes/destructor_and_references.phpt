--TEST--
ZE2 Destructing and references
--FILE--
<?php

class test1 {public $x;};
class test2 {public $x;};
class test3 {public $x;};
class test4 {public $x;};

$o1 = new test1;
$o2 = new test2;
$o3 = new test3;
$o4 = new test4;

$o3->x = &$o4;

$r1 = &$o1;

class once {}

$o = new once;
echo "Done\n";
?>
--EXPECT--
Done
