--TEST--
Stack overflow in mb_check_encoding() and mb_convert_encoding() with deeply nested arrays
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
zend.max_allowed_stack_size=256K
--FILE--
<?php
$deep = 'x';
for ($i = 0; $i < 30000; $i++) {
    $deep = [$deep];
}

/* Two nested arrays, so the second must not be visited after the first throws. */
foreach ([[$deep], [$deep, $deep]] as $input) {
    try {
        mb_check_encoding($input, 'UTF-8');
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
        var_dump($e->getPrevious());
    }
    try {
        mb_convert_encoding($input, 'UTF-8', 'ISO-8859-1');
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
        var_dump($e->getPrevious());
    }
}

unset($input, $e);
while (is_array($deep)) {
    $deep = $deep[0];
}
?>
--EXPECTF--
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
NULL
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
NULL
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
NULL
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
NULL
