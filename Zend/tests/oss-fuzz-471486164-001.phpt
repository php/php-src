--TEST--
OSS-Fuzz #471486164: get_property_ptr_ptr() on uninitialized hooked property
--FILE--
<?php

class C {
    public $a {
        get => $this->a;
        set { $this->a = &$value; }
    }
    public $x = 1;
}

$proxy = (new ReflectionClass(C::class))->newLazyProxy(function ($proxy) {
    $proxy->a = 1;
    return new C;
});
var_dump($proxy->x);

?>
--EXPECT--
int(1)
