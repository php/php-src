--TEST--
JIT ASSIGN_DIM_OP: Undefined variable and index with exception
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
set_error_handler(function($_, $m){
    throw new Exception($m);
});
function test1() {
    $res = $a[$undef] = null;
}
function test2() {
    $res = $a[$undef] += 1;
}
try {
    test1();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Undefined variable $undef
Undefined variable $a
