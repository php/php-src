--TEST--
GH-24081 (User opcode handlers resume execution against a stale frame under ZEND_VM_KIND_TAILCALL)
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_opcode_in_user_handler=ZEND_ADD
--FILE--
<?php
function f($a, $b) {
    $r = $a + $b;
    return strlen("x") + $r;
}

var_dump(f(1, 2));
?>
--EXPECTF--
<!-- init '%s' -->
<!-- init f() -->
<!-- opcode: 'ZEND_ADD' in user handler -->
<!-- opcode: 'ZEND_ADD' in user handler -->
<!-- init var_dump() -->
int(4)
