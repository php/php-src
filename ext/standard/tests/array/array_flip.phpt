--TEST--
basic array_flip test
--FILE--
<?php
$trans = array("a" => 1, 
               "b" => 1, 
               "c" => 2, 
               "z" => 0, 
               "d" => TRUE, 
               "E" => FALSE, 
               "F" => NULL, 
               0 => "G", 
               1 => "h", 
               2 => "i");
$trans = array_flip($trans);
var_dump($trans);

var_dump(array_flip());
var_dump(array_flip(array()));
var_dump(array_flip(array(1)));
var_dump(array_flip(array(array())));

echo "Done\n";
?>
--EXPECTF--
Warning: array_flip(): Can only flip STRING and INTEGER values! in %s on line %d

Warning: array_flip(): Can only flip STRING and INTEGER values! in %s on line %d

Warning: array_flip(): Can only flip STRING and INTEGER values! in %s on line %d
array(6) {
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [0]=>
  string(1) "z"
  ["G"]=>
  int(0)
  ["h"]=>
  int(1)
  ["i"]=>
  int(2)
}

Warning: Wrong parameter count for array_flip() in %s on line %d
NULL
array(0) {
}
array(1) {
  [1]=>
  int(0)
}

Warning: array_flip(): Can only flip STRING and INTEGER values! in %s on line %d
array(0) {
}
Done
