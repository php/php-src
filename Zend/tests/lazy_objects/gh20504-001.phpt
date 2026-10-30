--TEST--
GH-20504: Assertion failure in zend_get_property_guard() when lazy proxy adds magic method - isset
--CREDITS--
vi3tL0u1s
--FILE--
<?php

class RealInstance {
    public $_;
}
class Proxy extends RealInstance {
    public function __isset($name) {
        return isset($this->$name['']);
    }
}
$rc = new ReflectionClass(Proxy::class);
$obj = $rc->newLazyProxy(function () {
    return new RealInstance;
});
var_dump(isset($obj->name['']));

?>
--EXPECT--
bool(false)
