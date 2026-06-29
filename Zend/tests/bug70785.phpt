--TEST--
Bug #70785 (Infinite loop due to exception during identical comparison)
--FILE--
<?php

set_error_handler(function($no, $msg) {
    throw new Exception($msg);
});

function smart_branch() {
    if ($a === null) { // ZEND_VM_SMART_BRANCH
        return 'branch';
    }
    return 'no branch';
}

function next_opcode() {
    $c === 3; // ZEND_VM_NEXT_OPCODE
    return 'done';
}

try {
    smart_branch();
} catch (Exception $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    next_opcode();
} catch (Exception $e) {
    echo $e->getMessage(), PHP_EOL;
}
echo "okey\n";
?>
--EXPECT--
Undefined variable $a
Undefined variable $c
okey
