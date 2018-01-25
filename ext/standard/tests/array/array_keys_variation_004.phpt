--TEST--
Test array_keys() function (variation - 4)
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
  var_dump(array_keys($types_arr, $value, TRUE));
}

echo "Done\n";
?>
--EXPECTF--
*** Testing array_keys() on all the types other than arrays ***
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(2)
}
array(1) {
  [0]=>
  int(3)
}
array(1) {
  [0]=>
  string(3) "php"
}
array(1) {
  [0]=>
  string(0) ""
}
Done
