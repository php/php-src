--TEST--
GH-20504: Assertion failure in zend_get_property_guard() when lazy proxy adds magic method - get
--FILE--
<?php

class RealInstance {
    public $_;
}
class Proxy extends RealInstance {
    public function __get($name) {
        return $this->$name;
    }
}
$rc = new ReflectionClass(Proxy::class);
$obj = $rc->newLazyProxy(function () {
    return new RealInstance;
});
var_dump($obj->name);

?>
--EXPECTF--
Warning: Undefined property: RealInstance::$name in %s on line %d
NULL
