--TEST--
Lazy objects: debug_zval_dump does not initialize object
--FILE--
<?php

class C {
    public int $a;
    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 1;
    }
}

$reflector = new ReflectionClass(C::class);

print "# Ghost:\n";

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

debug_zval_dump($obj);
$reflector->initializeLazyObject($obj);
debug_zval_dump($obj);

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

debug_zval_dump($obj);
$reflector->initializeLazyObject($obj);
debug_zval_dump($obj);

--EXPECTF--
# Ghost:
lazy ghost object(C)#%d (0) refcount(2){
  ["a"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
object(C)#%d (1) refcount(2){
  ["a"]=>
  int(1)
}
# Proxy:
lazy proxy object(C)#%d (0) refcount(2){
  ["a"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
lazy proxy object(C)#%d (1) refcount(2){
  ["instance"]=>
  object(C)#%d (1) refcount(2){
    ["a"]=>
    int(1)
  }
}
