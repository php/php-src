--TEST--
Tracing JIT: preserve the next opline when handling a VM interrupt
--DESCRIPTION--
The object comparison raises an interrupt; the observer INI setting installs
an interrupt callback without enabling observer events. While recording the
caller, the backward jump must save its target before the interrupt helper
reloads the opline. Otherwise it revisits the already executed call opcode,
whose pending call frame has been consumed.

At the time of writing this was failing on the TAILCALL VM, but this should
pass on all VMs.
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_func=1
opcache.jit_hot_loop=1
zend_test.observer.set_vm_interrupt_on_begin=1
--FILE--
<?php
function trigger(VmInterruptComparable $left, VmInterruptComparable $right): void {
    // Raise an interrupt without an internal call that would handle it immediately.
    $result = $left == $right;
}

$left = new VmInterruptComparable(1);
$right = new VmInterruptComparable(2);
foreach ([1, 2, 3] as $i) {
    trigger($left, $right);
}
echo "Done\n";
?>
--EXPECT--
Done
