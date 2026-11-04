--TEST--
Function JIT: decode the VM-enter bit after an interrupt callback
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=1201
opcache.jit_buffer_size=64M
zend_test.observer.set_vm_interrupt_on_begin=1
--FILE--
<?php
function compare($left, $right) {
    if ($left == $right) {
        return 11;
    }
    return 22;
}

var_dump(compare(new VmInterruptComparable(1), new VmInterruptComparable(2)));
?>
--EXPECT--
int(22)
