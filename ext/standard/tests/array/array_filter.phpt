--TEST--
basic array_filter test
--FILE--
<?php
function odd($var)
{
   return($var & 1);
}

function even($var)
{
   return(!($var & 1));
}

$array1 = array("a"=>1, "b"=>2, "c"=>3, "d"=>4, "e"=>5);
$array2 = array(6, 7, 8, 9, 10, 11, 12, 0);
$array3 = array(TRUE, FALSE, NULL);

echo "Odd :\n";
var_dump(array_filter($array1, "odd"));
var_dump(array_filter($array2, "odd"));
var_dump(array_filter($array3, "odd"));
echo "Even:\n";
var_dump(array_filter($array1, "even"));
var_dump(array_filter($array2, "even"));
var_dump(array_filter($array3, "even"));

var_dump(array_filter(array()));

?>
--EXPECT--
Odd :
array(3) {
  ["a"]=>
  int(1)
  ["c"]=>
  int(3)
  ["e"]=>
  int(5)
}
array(3) {
  [1]=>
  int(7)
  [3]=>
  int(9)
  [5]=>
  int(11)
}
array(1) {
  [0]=>
  bool(true)
}
Even:
array(2) {
  ["b"]=>
  int(2)
  ["d"]=>
  int(4)
}
array(5) {
  [0]=>
  int(6)
  [2]=>
  int(8)
  [4]=>
  int(10)
  [6]=>
  int(12)
  [7]=>
  int(0)
}
array(2) {
  [1]=>
  bool(false)
  [2]=>
  NULL
}
array(0) {
}
