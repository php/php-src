--TEST--
OSS-Fuzz #481017027: Missing zend_fe_fetch_object_helper deref
--FILE--
<?php

class C {
    public $y;
}

function test($obj, $name) {
    foreach ($obj as $$name) {
        var_dump($$name);
    }
}

$y = 42;
$obj = new C;
$obj->y = &$y;
test($obj, '');

?>
--EXPECT--
int(42)
