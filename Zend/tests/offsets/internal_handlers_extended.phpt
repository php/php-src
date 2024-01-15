--TEST--
Internal handlers that do not explicitly support userland ArrayAccess
--EXTENSIONS--
zend_test
--FILE--
<?php

class NoImplement extends DimensionHandlersNoArrayAccess {}

class DoImplement extends DimensionHandlersNoArrayAccess implements ArrayAccess {
    public function offsetSet($offset, $value): void {
        var_dump(__METHOD__);
        var_dump($offset);
        var_dump($value);
    }
    public function offsetGet($offset): mixed {
        var_dump(__METHOD__);
        var_dump($offset);
        return "dummy";
    }
    public function offsetUnset($offset): void {
        var_dump(__METHOD__);
        var_dump($offset);
    }
    public function offsetExists($offset): bool {
        var_dump(__METHOD__);
        var_dump($offset);
        return true;
    }
}

$no = new NoImplement();
$do = new DoImplement();

$no['foo'];
var_dump($no);

$do['foo'];
var_dump($do);

?>
--EXPECTF--
object(NoImplement)#1 (7) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(0)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
object(DoImplement)#2 (7) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(0)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
