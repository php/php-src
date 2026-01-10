--TEST--
Lazy objects: resetAsLazy*() can make a real instance lazy
--FILE--
<?php

class Obj {
    public function __construct(
        public string $name,
    ) {}
}

$r = new ReflectionClass(Obj::class);

$obj1 = new Obj('obj1');
$r->resetAsLazyProxy($obj1, function () use (&$obj2) {
    $obj2 = new Obj('obj2');
    return $obj2;
});
$r->initializeLazyObject($obj1);
$r->resetAsLazyProxy($obj2, function () {
    return new Obj('obj3');
});
var_dump($obj1->name);

?>
==DONE==
--EXPECT--
string(4) "obj3"
==DONE==
