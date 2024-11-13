--TEST--
Cloning stdClass
--FILE--
<?php

$x[] = clone new stdclass;
$x[] = clone new stdclass;
$x[] = clone new stdclass;

$x[0]->a = 1;

var_dump($x);

?>
--EXPECTF--
array(3) {
  [0]=>
  object(stdClass)#%d (1) {
    ["a"]=>
    int(1)
  }
  [1]=>
  object(stdClass)#%d (0) {
  }
  [2]=>
  object(stdClass)#%d (0) {
  }
}
