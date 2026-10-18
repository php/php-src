--TEST--
JIT: an interrupt after a frameless call does not run the call again (tracing JIT)
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
zend_test.observer.set_vm_interrupt_on_begin=1
--FILE--
<?php
function f($s) {
    $c = 0;
    for ($i = 0; $i < 3000; $i++) {
        if (str_contains($s . $i, '7')) {
            $c++;
        }
    }
    return $c;
}
var_dump(f("abc"));
var_dump(f("abc"));
?>
--EXPECT--
int(813)
int(813)
