--TEST--
JIT: FETCH_OBJ 008
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
class A {
    public string $prop = "";
}

class B {
    public function __toString() {
        global $a;
        $a->prop = "A $e B";
        $a->prop->prop . $a->prop = "C";
        return "test";
    }
}

$a = new A;
$a->prop = new B;
?>
DONE
--EXPECTF--
Warning: Undefined variable $e in %sfetch_obj_008.php on line 9

Warning: Attempt to read property "prop" on string in %sfetch_obj_008.php on line 10
DONE
