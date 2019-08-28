--TEST--
Cloning StdClass
--FILE--
<?php

$x[] = clone new StdClass;
$x[] = clone new StdClass;
$x[] = clone new StdClass;

$x[0]->a = 1;

var_dump($x);

?>
--EXPECTF--
array(3) {
  [0]=>
  object(StdClass)#%d (1) {
    ["a"]=>
    int(1)
  }
  [1]=>
  object(StdClass)#%d (0) {
  }
  [2]=>
  object(StdClass)#%d (0) {
  }
}
