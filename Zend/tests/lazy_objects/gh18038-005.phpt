--TEST--
GH-18038 005: Lazy proxy calls magic methods twice
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $_;
    public function __isset($name) {
        var_dump(__METHOD__);
        return isset($this->$name['']);
    }
}

$rc = new ReflectionClass(C::class);

$obj = $rc->newLazyProxy(function () {
    echo "init\n";
    return new C;
});

var_dump(isset($obj->prop['']));

?>
--EXPECT--
string(10) "C::__isset"
init
bool(false)
