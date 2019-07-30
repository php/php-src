--TEST--
By-ref unpacking of a nested access
--FILE--
<?php

function inc(&$var) {
    $var++;
}

$ary = [[1]];
inc(...$ary[0]);
var_dump($ary);

?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
}
