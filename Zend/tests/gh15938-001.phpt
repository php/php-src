--TEST--
GH-15938 001: ASSIGN_OBJ_OP: Dynamic property may be undef by __toString(), leaking the new value
--ENV--
LEN=10
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $a;
}

$obj = new C();
$obj->b = str_repeat('c', (int)getenv('LEN'));
$obj->b .= new class {
    function __toString() {
        global $obj;
        unset($obj->b);
        return str_repeat('d', (int)getenv('LEN'));
    }
};

var_dump($obj);

?>
==DONE==
--EXPECTF--
object(C)#%d (1) {
  ["a"]=>
  NULL
}
==DONE==
