--TEST--
Accessing members of standard object through of variable variable
--FILE--
<?php

error_reporting(E_ALL);

$test = 'stdclass';

$$test->a =& $$test;
$$test->a->b[] = 2;

var_dump($$test);

?>
--EXPECTF--
Strict Standards: Creating default object from empty value in %s on line %d
object(stdClass)#1 (2) {
  ["a"]=>
  &object(stdClass)#1 (2) {
    ["a"]=>
    *RECURSION*
    ["b"]=>
    array(1) {
      [0]=>
      int(2)
    }
  }
  ["b"]=>
  array(1) {
    [0]=>
    int(2)
  }
}
--UEXPECTF--
Strict Standards: Creating default object from empty value in %s on line %d
object(stdClass)#1 (2) {
  [u"a"]=>
  &object(stdClass)#1 (2) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    array(1) {
      [0]=>
      int(2)
    }
  }
  [u"b"]=>
  array(1) {
    [0]=>
    int(2)
  }
}
