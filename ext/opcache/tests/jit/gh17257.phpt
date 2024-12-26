--TEST--
GH-17257 (SEGV ext/opcache/jit/zend_jit_vm_helpers.c)
--EXTENSIONS--
opcache
--INI--
opcache.jit_buffer_size=32M
opcache.jit=1254
opcache.jit_hot_func=1
--FILE--
<?php
function get_const() {
}
function test() {
    call_user_func('get_const', 1); // need an extra arg to trigger the issue
}
function main(){
    for ($i = 0; $i < 10; $i++) {
        test();
    }
    echo "Done\n";
}
main();
?>
--EXPECT--
Done
