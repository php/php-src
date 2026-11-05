--TEST--
GH-15938
--FILE--
<?php

class C {
    public $a;
}

$obj = new C();
$obj->a = '';
$obj->a .= new class {
    function __toString() {
        global $obj;
        $obj = null;
        return 'str';
    }
};

?>
--EXPECTF--
Fatal error: Uncaught Error: Attempt to assign property "a" on null in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
