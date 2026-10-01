--TEST--
Observer: VM interrupt during tailcall helper dispatch
--DESCRIPTION--
This exercises a VM interrupt raised while an opcode handler dispatches to an
extra-argument helper. On the tailcall VM, the helper may return an opline
tagged with ZEND_VM_ENTER_BIT; treating that tagged value as a zend_op * before
tailcalling the next handler can crash.
--EXTENSIONS--
zend_test
--INI--
opcache.jit=0
zend_test.observer.set_vm_interrupt_on_begin=1
--FILE--
<?php
function trigger(VmInterruptComparable $left, VmInterruptComparable $right): object
{
    if ($left < $right) {
        return new Exception();
    }
    return new stdClass();
}

echo get_class(trigger(new VmInterruptComparable(2), new VmInterruptComparable(1))), "\n";
?>
--EXPECT--
stdClass
