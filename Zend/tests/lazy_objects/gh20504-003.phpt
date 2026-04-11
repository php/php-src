--TEST--
GH-20504: Assertion failure in zend_get_property_guard() when lazy proxy adds magic method - set
--FILE--
<?php

#[AllowDynamicProperties]
class RealInstance {
    public $_;
}
class Proxy extends RealInstance {
    public function __set($name, $value) {
        $this->$name = $value;
    }
}
$rc = new ReflectionClass(Proxy::class);
$obj = $rc->newLazyProxy(function () {
    return new RealInstance;
});
$obj->name = 0;

var_dump($obj);

?>
--EXPECTF--
lazy proxy object(Proxy)#%d (1) {
  ["instance"]=>
  object(RealInstance)#%d (2) {
    ["_"]=>
    NULL
    ["name"]=>
    int(0)
  }
}
