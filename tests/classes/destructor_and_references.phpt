--TEST--
Destucting and references
--FILE--
<?php

class test1 {var $x;};
class test2 {var $x;};
class test3 {var $x;};
class test4 {var $x;};

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