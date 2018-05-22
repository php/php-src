--TEST--
Covariant return-by-ref constraints
--FILE--
<?php

class A implements ArrayAccess {
    public $foo = array();

    public function &offsetGet($n) {
        return $this->foo[$n];
    }

    public function offsetSet($n, $v) {
    }
    public function offsetUnset($n) {
    }
    public function offsetExists($n) {
    }
}

$a = new A;

$a['foo']['bar'] = 2;

var_dump($a);

?>
==DONE==
--EXPECT--
object(A)#1 (1) {
  ["foo"]=>
  array(1) {
    ["foo"]=>
    array(1) {
      ["bar"]=>
      int(2)
    }
  }
}
==DONE==
