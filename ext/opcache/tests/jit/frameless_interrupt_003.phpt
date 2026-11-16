--TEST--
Properly deoptimize trace before frameless call interrupt
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=32M
zend_test.observer.enabled=1
zend_test.observer.show_output=0
zend_test.observer.observe_function_names=str_contains
zend_test.observer.set_vm_interrupt_on_begin_without_callback=1
--FILE--
<?php
function test() {
    $sum = 0;
    for ($i = 0; $i < 3000; $i++) {
        $sum += $i;
        str_contains($i, 'x');
    }
    return $sum;
}

var_dump(test());
var_dump(test());
?>
--EXPECT--
int(4498500)
int(4498500)
