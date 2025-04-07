--TEST--
Lazy objects: properties with no default values are left uninitialized
--FILE--
<?php

class C {
    public $a;
    public int $b;

    public function __construct() {
        var_dump(__METHOD__);
    }
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

var_dump($obj);
var_dump($obj->a);
try {
    var_dump($obj->b);
} catch (Error $e) {
    printf("%s\n", $e);
}
var_dump($obj);
--EXPECTF--
lazy ghost object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
string(11) "initializer"
string(14) "C::__construct"
NULL
Error: Typed property C::$b must not be accessed before initialization in %s:%d
Stack trace:
#0 {main}
object(C)#%d (1) {
  ["a"]=>
  NULL
  ["b"]=>
  uninitialized(int)
}
