--TEST--
GH-18038 012: Lazy proxy calls magic methods twice
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $_;
    public function __set($name, $value) {
        var_dump(__METHOD__);
        $this->$name = $value * 2;
    }
}

$rc = new ReflectionClass(C::class);

$obj = $rc->newLazyGhost(function () {
    echo "init\n";
});

$obj->prop = 1;
var_dump($obj->prop);

?>
--EXPECT--
string(8) "C::__set"
init
int(2)
