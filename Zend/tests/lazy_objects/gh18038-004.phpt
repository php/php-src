--TEST--
GH-18038 004: Lazy proxy calls magic methods twice
--FILE--
<?php

#[AllowDynamicProperties]
class RealInstance {
    public $_;
    public function __get($name) {
        global $obj;
        var_dump(get_class($this)."::".__FUNCTION__);
        var_dump($obj->$name);
        return $this->$name;
    }
}

#[AllowDynamicProperties]
class Proxy extends RealInstance {
    public function __get($name) {
        var_dump(get_class($this)."::".__FUNCTION__);
        return $this->$name;
    }
}

$rc = new ReflectionClass(Proxy::class);

$obj = $rc->newLazyProxy(function () {
    echo "init\n";
    return new RealInstance;
});

$real = $rc->initializeLazyObject($obj);
var_dump($real->prop);

?>
--EXPECTF--
init
string(19) "RealInstance::__get"
string(12) "Proxy::__get"

Warning: Undefined property: RealInstance::$prop in %s on line %d
NULL

Warning: Undefined property: RealInstance::$prop in %s on line %d
NULL
