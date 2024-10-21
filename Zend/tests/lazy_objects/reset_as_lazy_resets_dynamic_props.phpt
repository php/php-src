--TEST--
Lazy objects: resetAsLazy resets dynamic props
--FILE--
<?php

class Canary {
    public function __destruct() {
        var_dump(__METHOD__);
    }
}

#[\AllowDynamicProperties]
class C {
    public $b;
    public function __construct() {
        $this->a = new Canary();
    }
}

$reflector = new ReflectionClass(C::class);

print "# Ghost:\n";

$obj = new C();
$reflector->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

var_dump($obj);
var_dump($obj->a);
var_dump($obj);

print "# Proxy:\n";

$obj = new C();
$reflector->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C();
});

var_dump($obj);
var_dump($obj->a);
var_dump($obj->a);
var_dump($obj);

--EXPECTF--
# Ghost:
string(18) "Canary::__destruct"
lazy ghost object(C)#%d (%d) {
}
string(11) "initializer"
object(Canary)#%d (0) {
}
object(C)#%d (2) {
  ["b"]=>
  NULL
  ["a"]=>
  object(Canary)#%d (0) {
  }
}
# Proxy:
string(18) "Canary::__destruct"
string(18) "Canary::__destruct"
lazy proxy object(C)#%d (%d) {
}
string(11) "initializer"
object(Canary)#%d (0) {
}
object(Canary)#%d (0) {
}
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (2) {
    ["b"]=>
    NULL
    ["a"]=>
    object(Canary)#%d (0) {
    }
  }
}
string(18) "Canary::__destruct"
