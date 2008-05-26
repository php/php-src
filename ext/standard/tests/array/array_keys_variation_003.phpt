--TEST--
Test array_keys() function (variation - 3)
--FILE--
<?php

echo "*** Testing array_keys() on all the types other than arrays ***\n";
$types_arr = array(
  TRUE => TRUE,
  FALSE => FALSE,
  1 => 1,
  0 => 0,
  -1 => -1, 
  "1" => "1",
  "0" => "0",
  "-1" => "-1",
  NULL,
  array(),
  "php" => "php",
  "" => ""
);
$values = array(TRUE, FALSE, 1, 0, -1, "1", "0", "-1",  NULL, array(), "php", "");
foreach ($values as $value){
	var_dump($value);
	var_dump(array_keys($types_arr, $value)); 
}

echo "Done\n";
?>
--EXPECT--
*** Testing array_keys() on all the types other than arrays ***
bool(true)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(-1)
  [2]=>
  unicode(3) "php"
}
bool(false)
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  unicode(0) ""
}
int(1)
array(1) {
  [0]=>
  int(1)
}
int(0)
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(2)
  [2]=>
  unicode(3) "php"
  [3]=>
  unicode(0) ""
}
int(-1)
array(1) {
  [0]=>
  int(-1)
}
unicode(1) "1"
array(1) {
  [0]=>
  int(1)
}
unicode(1) "0"
array(1) {
  [0]=>
  int(0)
}
unicode(2) "-1"
array(1) {
  [0]=>
  int(-1)
}
NULL
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  unicode(0) ""
}
array(0) {
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
unicode(3) "php"
array(1) {
  [0]=>
  unicode(3) "php"
}
unicode(0) ""
array(2) {
  [0]=>
  int(2)
  [1]=>
  unicode(0) ""
}
Done
