--TEST--
zend_analyze_calls(): call_stack overflow when dead code elimination removed the DO_FCALLs
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
--ENV--
USE_ZEND_ALLOC=0
USE_TRACKED_ALLOC=1
--FILE--
<?php
function test() {
    new A(new B(new C(new D(match ([]) { 1 => 2 }))));
}
echo "OK\n";
?>
--EXPECT--
OK
