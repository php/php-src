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
--EXPECT--
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
--UEXPECT--
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
