--TEST--
GH-20504: Assertion failure in zend_get_property_guard() when lazy proxy adds magic method - proxy defines __isset(), both have guards
--FILE--
<?php

class RealInstance {
    public $_;
    public function __get($name) {
        printf("%s::%s\n", static::class, __FUNCTION__);
    }
}
class Proxy extends RealInstance {
    public function __isset($name) {
        printf("%s::%s\n", static::class, __FUNCTION__);
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
Proxy::__isset
Proxy::__get
bool(false)
