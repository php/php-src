--TEST--
Accessing members of standard object through of variable variable
--FILE--
<?php

error_reporting(E_ALL);

$stdclass = new stdClass;
$test = 'stdclass';

$$test->a =& $$test;
$$test->a->b[] = 2;

var_dump($$test);

?>
--EXPECTF--
object(stdClass)#1 (2) {
  ["a"]=>
  *RECURSION*
  ["b"]=>
  array(1) {
    [0]=>
    int(2)
  }
}
