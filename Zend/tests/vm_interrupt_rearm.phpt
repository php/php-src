--TEST--
VM interrupt re-armed by the interrupt handler must not stop execution from making progress
--DESCRIPTION--
zend_test.rearm_vm_interrupt installs a zend_interrupt_function that sets
EG(vm_interrupt) again every time it runs, so an interrupt is always pending.
After handling an interrupt the VM has to run the next opline before it checks
the flag again, otherwise it calls the handler forever (or recurses until the
stack overflows).
--EXTENSIONS--
zend_test
--INI--
zend_test.rearm_vm_interrupt=1
--FILE--
<?php
function inc(int $x): int {
    return $x + 1;
}

$n = 0;
for ($i = 0; $i < 1000; $i++) {
    $n = inc($n);
}
var_dump($n);
?>
--EXPECT--
int(1000)
