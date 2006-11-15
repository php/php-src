--TEST--
array_unshift() tests
--FILE--
<?php

$a = array();
$s = "";
var_dump(array_unshift($a, $s));
var_dump($a);
var_dump(array_unshift($s, $a));
var_dump($a);
var_dump(array_unshift($a, $a));
var_dump($a);

echo "Done\n";
?>
--EXPECTF--	
int(1)
array(1) {
  [0]=>
  string(0) ""
}

Warning: array_unshift(): The first argument should be an array in %s on line %d
bool(false)
array(1) {
  [0]=>
  string(0) ""
}
int(2)
array(2) {
  [0]=>
  array(1) {
    [0]=>
    string(0) ""
  }
  [1]=>
  string(0) ""
}
Done
