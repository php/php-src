--TEST--
Testing assign to property of an object in an array
--FILE--
<?php

$arr = array(new stdClass);

$arr[0]->a = clone $arr[0];
var_dump($arr);

$arr[0]->b = new $arr[0];
var_dump($arr);

$arr[0]->c = $arr[0]->a;
var_dump($arr);

?>
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#1 (1) {
    [u"a"]=>
    object(stdClass)#2 (0) {
    }
  }
}
array(1) {
  [0]=>
  object(stdClass)#1 (2) {
    [u"a"]=>
    object(stdClass)#2 (0) {
    }
    [u"b"]=>
    object(stdClass)#3 (0) {
    }
  }
}
array(1) {
  [0]=>
  object(stdClass)#1 (3) {
    [u"a"]=>
    object(stdClass)#2 (0) {
    }
    [u"b"]=>
    object(stdClass)#3 (0) {
    }
    [u"c"]=>
    object(stdClass)#2 (0) {
    }
  }
}
