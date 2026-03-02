--TEST--
filter_var_array() on array with reference
--EXTENSIONS--
filter
--FILE--
<?php

$array = ["123foo"];
$array2 = [&$array];
var_dump(filter_var_array($array2, FILTER_VALIDATE_INT));
var_dump($array);

?>
--EXPECT--
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    bool(false)
  }
}
array(1) {
  [0]=>
  bool(false)
}
