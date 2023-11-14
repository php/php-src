--TEST--
JIT ASSIGN_OBJ_OP: memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
class A {
    public string $prop = "222";
}

class B {
    public function __toString() {
        global $a;
        $a->prop .=  $a->prop . "leak";
        return "test";
    }
}

$a = new A;
$prop = &$a->prop;
$a->prop = new B;
var_dump($a);
?>
--EXPECT--
object(A)#1 (1) {
  ["prop"]=>
  &string(4) "test"
}
