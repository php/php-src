--TEST--
GH-18038 007: Lazy proxy calls magic methods twice
--FILE--
<?php

#[AllowDynamicProperties]
class RealInstance {
    public $_;
    public function __isset($name) {
        global $obj;
        var_dump(get_class($this)."::".__FUNCTION__);
        var_dump(isset($obj->$name['']));
        return isset($this->$name['']);
    }
}

#[AllowDynamicProperties]
class Proxy extends RealInstance {
    public function __isset($name) {
        var_dump(get_class($this)."::".__FUNCTION__);
        return isset($this->$name['']);
    }
}

$rc = new ReflectionClass(Proxy::class);

$obj = $rc->newLazyProxy(function () {
    echo "init\n";
    return new RealInstance;
});

$real = $rc->initializeLazyObject($obj);
var_dump(isset($real->prop['']));

?>
--EXPECT--
init
string(21) "RealInstance::__isset"
string(14) "Proxy::__isset"
bool(false)
bool(false)
