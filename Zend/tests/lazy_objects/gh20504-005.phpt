--TEST--
GH-20504: Assertion failure in zend_get_property_guard() when lazy proxy adds magic method - unset
--FILE--
<?php

class RealInstance {
    public $_;
}
class Proxy extends RealInstance {
    public function __unset($name) {
        unset($this->$name);
    }
}
$rc = new ReflectionClass(Proxy::class);
$obj = $rc->newLazyProxy(function () {
    return new RealInstance;
});
unset($obj->name);

var_dump($obj);

?>
--EXPECTF--
lazy proxy object(Proxy)#%d (1) {
  ["instance"]=>
  object(RealInstance)#%d (1) {
    ["_"]=>
    NULL
  }
}
