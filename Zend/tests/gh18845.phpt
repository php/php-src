--TEST--
GH-18845: Use-after-free of object through __isset() and globals
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $dummy;
    public function __isset($x) {
        $GLOBALS['c'] = null;
        return true;
    }
}

$c = new C;
var_dump($c->prop ?? 1);

$r = new ReflectionClass(C::class);
$c = $r->newLazyProxy(function () {
    $c = new C();
    $c->prop = 2;
    return $c;
});
var_dump($c->prop ?? 1);

?>
--EXPECT--
int(1)
int(2)
