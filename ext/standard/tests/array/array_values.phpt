--TEST--
array_values() tests
--FILE--
<?php

var_dump(array_values());
var_dump(array_values(array()));
var_dump(array_values(""));
var_dump(array_values(new stdclass));

$a = array(1, 2, 2.0, "asdasd", array(1,2,3));

var_dump(array_values($a));

echo "Done\n";
?>
--EXPECTF--	
Warning: array_values() expects exactly 1 parameter, 0 given in %s on line %d
NULL
array(0) {
}

Warning: array_values() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_values() expects parameter 1 to be array, object given in %s on line %d
NULL
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  float(2)
  [3]=>
  string(6) "asdasd"
  [4]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
Done
--UEXPECTF--
Warning: array_values() expects exactly 1 parameter, 0 given in %s on line %d
NULL
array(0) {
}

Warning: array_values() expects parameter 1 to be array, Unicode string given in %s on line %d
NULL

Warning: array_values() expects parameter 1 to be array, object given in %s on line %d
NULL
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  float(2)
  [3]=>
  unicode(6) "asdasd"
  [4]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
Done
