--TEST--
Tracing JIT: a jumping handler must not take the other branch after a VM interrupt
--DESCRIPTION--
The object comparison raises an interrupt, and the observer INI setting
installs an interrupt callback. The tracing JIT runs COALESCE through its VM
handler. Taking the jump, the handler checks for interrupts and runs the
callback, then returns the jump target with ZEND_VM_ENTER_BIT set. Without
clearing that bit, the opline guard after the handler fails and the trace
exits to the fall-through branch, so `??` returns its default even though
the key exists.

This fails on the CALL and TAILCALL VMs only; the HYBRID VM doesn't tag the
returned opline.
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
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
zend_test.observer.set_vm_interrupt_on_begin=1
--FILE--
<?php
function get(VmInterruptComparable $left, VmInterruptComparable $right, array $arr) {
    // Raise an interrupt without an internal call that would handle it immediately.
    $result = $left == $right;
    return $arr['x'] ?? 'MISSING';
}

$left = new VmInterruptComparable(1);
$right = new VmInterruptComparable(2);
for ($i = 0; $i < 5; $i++) {
    var_dump(get($left, $right, ['x' => $i]));
}
?>
--EXPECT--
int(0)
int(1)
int(2)
int(3)
int(4)
