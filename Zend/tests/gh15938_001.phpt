--TEST--
GH-15938
--FILE--
<?php

#[AllowDynamicProperties]
class C {}

$obj = new C();
// $obj->a = str_repeat('a', 10);
$obj->a .= new class {
    function __toString() {
        global $obj;
        unset($obj->a);
        return str_repeat('c', 10);
    }
};

var_dump($obj->a);

?>
--EXPECTF--
Warning: Undefined property: C::$a in %s on line %d
string(10) "cccccccccc"
