--TEST--
GH-15938 001: ASSIGN_OBJ_OP: Property ht may be freed by resetAsLazy*(), write happens on freed ht
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $a;
}

$obj = new C();
$obj->b = str_repeat('a', 10);
$obj->b .= new class {
    function __toString() {
        global $obj;
        $rc = new ReflectionClass($obj::class);
        $rc->resetAsLazyGhost($obj, function () {});
        return 'str';
    }
};

var_dump($obj);

?>
==DONE==
--EXPECTF--
lazy ghost object(C)#%d (0) {
}
==DONE==
