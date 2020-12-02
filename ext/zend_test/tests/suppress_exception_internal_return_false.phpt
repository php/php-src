--TEST--
Suppresion operator internal functions can return things other than null: false
--SKIPIF--
<?php if (!extension_loaded('zend_test')) die('skip: zend_test extension required'); ?>
--FILE--
<?php

$value = @zend_suppress_exception_internal_return_false();

var_dump($value);

try {
    zend_suppress_exception_internal_return_false();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

echo 'Done', PHP_EOL;
?>
--EXPECT--
bool(false)
Error: Shouldn't be seen
Done
