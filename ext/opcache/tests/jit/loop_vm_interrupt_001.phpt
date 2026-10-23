--TEST--
Tracing JIT: register-allocated loop variable must survive a VM interrupt raised in the loop body
--DESCRIPTION--
VmInterruptComparable's compare handler raises EG(vm_interrupt) without a
function call, so the interrupt is only noticed by the check that closes the
loop trace. That check must deoptimize (write $s back to its CV slot) before
entering the VM.
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=1
--FILE--
<?php
function sum(array $objs, VmInterruptComparable $t): float {
    $s = 0.0;
    foreach ($objs as $o) {
        $s += 1.5;
        // Only VmInterruptComparable's compare handler raises the interrupt.
        $r = $o == $t;
    }
    return $s;
}

$objs = array_fill(0, 200, new stdClass);
$objs[150] = new VmInterruptComparable(0);
var_dump(sum($objs, new VmInterruptComparable(0)));
?>
--EXPECT--
float(300)
