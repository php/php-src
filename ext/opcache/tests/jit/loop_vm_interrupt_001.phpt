--TEST--
Tracing JIT: register-allocated loop variable must survive a VM interrupt raised in the loop body
--DESCRIPTION--
Comparing two VmInterruptComparable objects sets EG(vm_interrupt) without a
PHP function call, so the JIT only notices the interrupt at the end of the
loop. Before handing control to the VM there, it has to write $s back to
memory, or the loop continues with a stale value.

This only shows up if $s is kept in a callee-saved register across the
comparison, which is why it's an int: x86_64 has no callee-saved floating
point registers, so a float would always be spilled to memory first.
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
function sum(array $objs, VmInterruptComparable $t): int {
    $s = 0;
    foreach ($objs as $o) {
        $s += 3;
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
int(600)
