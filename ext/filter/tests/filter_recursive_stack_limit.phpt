--TEST--
Stack overflow in filter_var() and filter_var_array() with deeply nested arrays
--EXTENSIONS--
filter
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

/* Two top-level entries: the second must not be visited once the
 * stack limit error has been thrown, so only one Error is thrown. */
try {
    filter_var([$deep, $deep], FILTER_DEFAULT, FILTER_REQUIRE_ARRAY);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
    var_dump($e->getPrevious());
}
try {
    filter_var_array(['a' => $deep, 'b' => $deep], ['a' => ['flags' => FILTER_REQUIRE_ARRAY], 'b' => ['flags' => FILTER_REQUIRE_ARRAY]]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
    var_dump($e->getPrevious());
}

unset($e);
while (is_array($deep)) {
    $deep = $deep[0];
}
?>
--EXPECTF--
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
NULL
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
NULL
