--TEST--
Lazy objects: resetAsLazy*() can reset initialized proxy
--FILE--
<?php

class Obj {
    public function __construct(
        public string $name,
    ) {}
}

$r = new ReflectionClass(Obj::class);

$obj = new Obj('obj1');
var_dump($obj);
$r->resetAsLazyProxy($obj, function () {
    return new Obj('obj2');
});
$r->initializeLazyObject($obj);
var_dump($obj);
$r->resetAsLazyProxy($obj, function () {
    return new Obj('obj3');
});
var_dump($obj);
$r->initializeLazyObject($obj);
var_dump($obj);

?>
==DONE==
--EXPECTF--
object(Obj)#%d (1) {
  ["name"]=>
  string(4) "obj1"
}
lazy proxy object(Obj)#%d (1) {
  ["instance"]=>
  object(Obj)#%d (1) {
    ["name"]=>
    string(4) "obj2"
  }
}
lazy proxy object(Obj)#%d (0) {
  ["name"]=>
  uninitialized(string)
}
lazy proxy object(Obj)#%d (1) {
  ["instance"]=>
  object(Obj)#%d (1) {
    ["name"]=>
    string(4) "obj3"
  }
}
==DONE==
