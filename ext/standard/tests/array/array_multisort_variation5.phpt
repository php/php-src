--TEST--
Test array_multisort() function : usage variation - testing with multiple array arguments
--FILE--
<?php
echo "*** Testing array_multisort() : Testing  all array sort specifiers ***\n";

$ar = array( 2, "aa" , "1");

array_multisort($ar, SORT_REGULAR, SORT_ASC);
var_dump($ar);

array_multisort($ar, SORT_STRING, SORT_ASC);
var_dump($ar);

array_multisort($ar, SORT_NUMERIC, SORT_ASC);
var_dump($ar);


?>
--EXPECT--
*** Testing array_multisort() : Testing  all array sort specifiers ***
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  int(2)
  [2]=>
  string(2) "aa"
}
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  int(2)
  [2]=>
  string(2) "aa"
}
array(3) {
  [0]=>
  string(2) "aa"
  [1]=>
  string(1) "1"
  [2]=>
  int(2)
}
