--TEST--
Observer: VM interrupt during tailcall return to caller
--DESCRIPTION--
This exercises a VM interrupt raised immediately before a user function returns
to a caller that invoked it through DO_FCALL. On the tailcall VM, the caller's
saved opline must point to the opcode after DO_FCALL before a pending interrupt
is handled.
--EXTENSIONS--
zend_test
--INI--
opcache.jit=0
zend_test.observer.set_vm_interrupt_on_begin=1
--FILE--
<?php
function interrupt_before_return(VmInterruptComparable $left, VmInterruptComparable $right): void
{
    $left < $right;
}

function call_interrupt_before_return(): void
{
    interrupt_before_return(new VmInterruptComparable(2), new VmInterruptComparable(1));
}

call_interrupt_before_return();
echo "ok\n";
?>
--EXPECT--
ok
