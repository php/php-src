--TEST--
Lazy objects: default values are initialized before calling initializer
--FILE--
<?php

class C {
    public $a = 1;
    public int $b = 2;

    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 3;
        $this->b = 4;
    }
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    var_dump($obj);
    $obj->__construct();
});

var_dump($obj);
var_dump($obj->a);
var_dump($obj);
--EXPECTF--
lazy ghost object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
string(11) "initializer"
object(C)#%d (2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
string(14) "C::__construct"
int(3)
object(C)#%d (2) {
  ["a"]=>
  int(3)
  ["b"]=>
  int(4)
}
