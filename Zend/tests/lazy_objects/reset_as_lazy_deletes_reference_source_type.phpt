--TEST--
Lazy objects: resetAsLazy deletes reference source type
--FILE--
<?php

class C {
    public int $a;

    public function __construct() {
        $this->a = 1;
    }
}

$reflector = new ReflectionClass(C::class);

print "# Ghost:\n";

$obj = new C();
$ref = &$obj->a;
try {
    $ref = 'string';
} catch (\Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}
$reflector->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

$ref = 'string';
var_dump($obj);
var_dump($obj->a);
var_dump($obj);

print "# Proxy:\n";

$obj = new C();
$ref = &$obj->a;
try {
    $ref = 'string';
} catch (\Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}
$reflector->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C();
});

$ret = 'string';
var_dump($obj);
var_dump($obj->a);
var_dump($obj->a);
var_dump($obj);

--EXPECTF--
# Ghost:
TypeError: Cannot assign string to reference held by property C::$a of type int
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
# Proxy:
TypeError: Cannot assign string to reference held by property C::$a of type int
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
