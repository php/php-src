--TEST--
GH-15938 003: ASSIGN_OBJ_OP: Object may be freed by __toString(), write happens on freed object
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

var_dump($obj);

?>
==DONE==
--EXPECT--
NULL
==DONE==
