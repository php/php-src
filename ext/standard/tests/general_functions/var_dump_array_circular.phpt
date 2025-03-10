--TEST--
Test var_dump() function with circular array
--FILE--
<?php

$a1 = [];
$a2 = [&$a1];
$a1[] =& $a2;

var_dump($a2);

?>
--EXPECT--
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    *RECURSION*
  }
}
