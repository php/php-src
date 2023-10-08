--TEST--
Test empty arrays with first added index being negative
--FILE--
<?php

$a = [];
$a[-5] = "-5";
$a[] = "after -5";

var_dump($a);
?>
--EXPECT--
array(2) {
  [-5]=>
  string(2) "-5"
  [-4]=>
  string(8) "after -5"
}
