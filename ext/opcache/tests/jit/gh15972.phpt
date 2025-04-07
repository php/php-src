--TEST--
GH-15972 (Assertion failure in ext/opcache/jit/zend_jit_vm_helpers.c with function JIT)
--EXTENSIONS--
opcache
--FILE--
<?php
function test(){
for($i = 0; $i < 2; $i++){
$a = @[3][$i];
}
}
test();
?>
DONE
--EXPECT--
DONE
