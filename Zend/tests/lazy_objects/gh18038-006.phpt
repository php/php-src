--TEST--
GH-18038 006: Lazy proxy calls magic methods twice
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $_;
    public function __isset($name) {
        var_dump(__METHOD__);
        return isset($this->$name['']);
    }
    public function __get($name) {
        var_dump(__METHOD__);
        return $this->$name[''];
    }
}

$rc = new ReflectionClass(C::class);

$obj = $rc->newLazyProxy(function () {
    echo "init\n";
    return new C;
});

var_dump(isset($obj->prop['']));

?>
--EXPECTF--
string(10) "C::__isset"
string(8) "C::__get"
init

Warning: Undefined property: C::$prop in %s on line %d

Warning: Trying to access array offset on null in %s on line %d
bool(false)
