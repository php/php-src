--TEST--
GH-18038 011: Lazy proxy calls magic methods twice
--FILE--
<?php

#[AllowDynamicProperties]
class RealInstance {
    public $_;
    public function __unset($name) {
        global $obj;
        var_dump(get_class($this)."::".__FUNCTION__);
        unset($this->$name);
    }
}

#[AllowDynamicProperties]
class Proxy extends RealInstance {
    public function __isset($name) {
        var_dump(get_class($this)."::".__FUNCTION__);
        unset($this->$name);
    }
}

$rc = new ReflectionClass(Proxy::class);

$obj = $rc->newLazyProxy(function () {
    echo "init\n";
    return new RealInstance;
});

$real = $rc->initializeLazyObject($obj);
unset($real->prop);
var_dump($obj);

?>
--EXPECTF--
init
string(21) "RealInstance::__unset"
lazy proxy object(Proxy)#%d (1) {
  ["instance"]=>
  object(RealInstance)#%d (1) {
    ["_"]=>
    NULL
  }
}
