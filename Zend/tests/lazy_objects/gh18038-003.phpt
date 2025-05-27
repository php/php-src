--TEST--
GH-18038 003: Lazy proxy calls magic methods twice
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $_;
    public function __get($name) {
        var_dump(__METHOD__);
        return $this->$name;
    }
}

$rc = new ReflectionClass(C::class);

$obj = $rc->newLazyProxy(function () {
    echo "init\n";
    return new C;
});

var_dump($obj->prop);

?>
--EXPECTF--
string(8) "C::__get"
init

Warning: Undefined property: C::$prop in %s on line %d
NULL
