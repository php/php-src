--TEST--
GH-18038 010: Lazy proxy calls magic methods twice
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $_;
    public function __unset($name) {
        var_dump(__METHOD__);
        unset($this->$name);
    }
}

$rc = new ReflectionClass(C::class);

$obj = $rc->newLazyProxy(function () {
    echo "init\n";
    return new C;
});

unset($obj->prop);
var_dump($obj);

?>
--EXPECTF--
string(10) "C::__unset"
init
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (1) {
    ["_"]=>
    NULL
  }
}
