--TEST--
array_shuffle(): Test return type and value for expected input 
--FILE--
<?php
/*
* proto bool shuffle ( array &$array )
* Function is implemented in ext/standard/array.c
*/
$numbers = range(1, 20);
echo "*** testing array_shuffle  \n";
$a = array();
var_dump(shuffle($a));
var_dump($a);
$a = array(1);
var_dump(shuffle($a));
var_dump($a);
$a = array(2 => 1);
var_dump(shuffle($a));
var_dump($a);
$a = array("a" => 1);
var_dump(shuffle($a));
var_dump($a);
$a = array(array(1, 2, 3));
var_dump(shuffle($a));
var_dump($a);
$a = array(1, 1, 1, 1);
var_dump(shuffle($a));
var_dump($a);
$arr1 = array(5 => 1, 6 => 2, 7 => 3, 8 => 9);
$arr2 = array(5 => 1, 6 => 2, 7 => 3, 8 => 9);
shuffle($arr1);
echo "this should be 0->...." . count(array_diff($arr1, $arr2)) . "\n";
echo "this should be 4->...." . count(array_intersect($arr1, $arr2)) . "\n";
$bigarray = range(1, 400);
shuffle($bigarray);
echo "this should be 400->...." . count($bigarray) . "\n";
echo "*** testing pass by reference  \n";
$original = $bigarray;
shuffle($bigarray);
$diffarray = array_diff_assoc($original, $bigarray);
if (count($diffarray) < 350) {
    // with 400 entries, the probability that 50 entries or more get the same
    // key-> value association should be so close to zero it wont happen in the lifetime of the
    // universe.
    echo "shuffled array seems to be similar to original\n";
    var_dump($original);
    var_dump($bigarray);
} else {
    echo "test passed \n";
}
?>
--EXPECT--
*** testing array_shuffle  
bool(true)
array(0) {
}
bool(true)
array(1) {
  [0]=>
  int(1)
}
bool(true)
array(1) {
  [0]=>
  int(1)
}
bool(true)
array(1) {
  [0]=>
  int(1)
}
bool(true)
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
bool(true)
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(1)
}
this should be 0->....0
this should be 4->....4
this should be 400->....400
*** testing pass by reference  
test passed 