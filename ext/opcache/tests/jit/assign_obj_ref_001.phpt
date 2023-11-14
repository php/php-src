--TEST--
JIT ASSIGN_OBJ_REF: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function test() {
    $obj = new stdClass;
    $obj->prop =& $obj;
    var_dump($obj->prop);
}
test();
?>
--EXPECT--
object(stdClass)#1 (1) {
  ["prop"]=>
  *RECURSION*
}
