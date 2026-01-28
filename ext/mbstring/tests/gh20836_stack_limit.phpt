--TEST--
GH-20836 (Stack overflow in mb_convert_variables with recursive array references, stack limit case)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (ini_get('zend.max_allowed_stack_size') === false) {
    die('skip No stack limit support');
}
if (getenv('SKIP_ASAN')) {
    die('skip ASAN needs different stack limit setting due to more stack space usage');
}
?>
--INI--
zend.max_allowed_stack_size=128K
--FILE--
<?php

function createDeepArray($depth) {
    if ($depth <= 0) {
        return 'deep value';
    }
    return ['nested' => createDeepArray($depth - 1)];
}

// Create a deeply nested array that will trigger stack limit
$deepArray = createDeepArray(15000);

mb_convert_variables('utf-8', 'utf-8', $deepArray);

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion? in %s:%d
Stack trace:
#0 %s(%d): mb_convert_variables('utf-8', 'utf-8', Array)
#1 {main}
  thrown in %s on line %d
