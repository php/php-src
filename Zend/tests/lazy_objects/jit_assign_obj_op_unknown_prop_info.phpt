--TEST--
Lazy objects: JIT: ASSIGN_OBJ_OP with unknown prop info
--FILE--
<?php

class C {
    // Private prop so that prop_info is not inferred
    private int $a;
    public int $b;
    function __construct() {
        $this->a = 1;
        $this->b = 2;
    }
    function test(object $obj) {
        $obj->a += 1;
    }
}

$reflector = new ReflectionClass(C::class);

for ($i = 0; $i < 2; $i++) {
    $obj = $reflector->newLazyGhost(function ($obj) {
        var_dump("initializer");
        $obj->__construct();
    });
    // Call via reflection to avoid inlining.
    // - test() handlers are executed once, and prime the runtime cache
    // - On subsequent calls, the JIT'ed code is used, and we enter the valid runtime cache path
    $reflector->getMethod('test')->invoke($obj, $obj);
    var_dump($obj);
}

--EXPECTF--
string(11) "initializer"
object(C)#%d (2) {
  ["a":"C":private]=>
  int(2)
  ["b"]=>
  int(2)
}
string(11) "initializer"
object(C)#%d (2) {
  ["a":"C":private]=>
  int(2)
  ["b"]=>
  int(2)
}
