--TEST--
Suppresion operator internal functions can return things other than null: true
--SKIPIF--
<?php if (!extension_loaded('zend_test')) die('skip: zend_test extension required'); ?>
--XFAIL--
TBD if should return false, null, or true
--FILE--
<?php

$value = @zend_suppress_exception_internal_return_true();

var_dump($value);

try {
    zend_suppress_exception_internal_return_true();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

echo 'Done', PHP_EOL;
?>
--EXPECT--
bool(true)
Error: Shouldn't be seen
Done
