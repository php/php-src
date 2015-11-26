--TEST--
basic array_diff_key test
--FILE--
<?php
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red", "");
$array2 = array("a" => "green", "yellow", "red", TRUE);
$array3 = array("red", "a"=>"brown", "");
$result[] = array_diff_key($array1, $array2);
$result[] = array_diff_key($array1, $array3);
$result[] = array_diff_key($array2, $array3);
$result[] = array_diff_key($array1, $array2, $array3);

var_dump($result);

?>
--EXPECT--
array(4) {
  [0]=>
  array(2) {
    ["b"]=>
    string(5) "brown"
    ["c"]=>
    string(4) "blue"
  }
  [1]=>
  array(2) {
    ["b"]=>
    string(5) "brown"
    ["c"]=>
    string(4) "blue"
  }
  [2]=>
  array(1) {
    [2]=>
    bool(true)
  }
  [3]=>
  array(2) {
    ["b"]=>
    string(5) "brown"
    ["c"]=>
    string(4) "blue"
  }
}
