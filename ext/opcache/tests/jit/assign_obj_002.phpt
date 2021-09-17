--TEST--
JIT ASSIGN_OBJ: Assign undefined vatiable to property
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit=function
--FILE--
<?php
class Test {
    public $prop;
}
function test() {
    $o = new Test;
    $o->prop = $undef;
    var_dump($o->prop);
}
test();
?>
DONE
--EXPECTF--
Warning: Undefined variable $undef in %sassign_obj_002.php on line 7
NULL
DONE
