--TEST--
Lazy objects: readonly properties can be lazily initialized
--FILE--
<?php

class C {
    public readonly int $a;

    public function __construct() {
        $this->a = 1;
    }
}

print "# Ghost:\n";

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

var_dump($obj);
var_dump($obj->a);
var_dump($obj);

print "# Virtual:\n";

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C();
});

var_dump($obj);
var_dump($obj->a);
var_dump($obj->a);
var_dump($obj);

--EXPECTF--
# Ghost:
lazy ghost object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
string(11) "initializer"
int(1)
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
# Virtual:
lazy proxy object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
string(11) "initializer"
int(1)
int(1)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (1) {
    ["a"]=>
    int(1)
  }
}
