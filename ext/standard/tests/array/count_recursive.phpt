--TEST--
count
--POST--
--GET--
--FILE--
<?php
print "Testing arrays...\n";
$arr = array(1, array(3, 4, array(6, array(8))));
print "COUNT_NORMAL: should be 2, is ".count($arr, COUNT_NORMAL)."\n";
print "COUNT_RECURSIVE: should be 8, is ".count($arr, COUNT_RECURSIVE)."\n";

print "Testing hashes...\n";
$arr = array("a" => 1, "b" => 2, array("c" => 3, array("d" => 5)));
print "COUNT_NORMAL: should be 3, is ".count($arr, COUNT_NORMAL)."\n";
print "COUNT_RECURSIVE: should be 6, is ".count($arr, COUNT_RECURSIVE)."\n";

print "Testing strings...\n";
print "COUNT_NORMAL: should be 1, is ".count("string", COUNT_NORMAL)."\n";
print "COUNT_RECURSIVE: should be 1, is ".count("string", COUNT_RECURSIVE)."\n";

print "Testing various types with no second argument.\n";
print "COUNT_NORMAL: should be 1, is ".count("string")."\n";
print "COUNT_NORMAL: should be 2, is ".count(array("a", array("b")))."\n";
?>
--EXPECT--
Testing arrays...
COUNT_NORMAL: should be 2, is 2
COUNT_RECURSIVE: should be 8, is 8
Testing hashes...
COUNT_NORMAL: should be 3, is 3
COUNT_RECURSIVE: should be 6, is 6
Testing strings...
COUNT_NORMAL: should be 1, is 1
COUNT_RECURSIVE: should be 1, is 1
Testing various types with no second argument.
COUNT_NORMAL: should be 1, is 1
COUNT_NORMAL: should be 2, is 2 
