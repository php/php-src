--TEST--
GH-12835: call->extra_named_params leaks on internal __call
--EXTENSIONS--
zend_test
--FILE--
<?php
$obj = new _ZendTestMagicCall;
var_dump($obj->test('a', 'b', c: 'c'));
?>
--EXPECT--
array(2) {
  [0]=>
  string(4) "test"
  [1]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    ["c"]=>
    string(1) "c"
  }
}
