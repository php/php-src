--TEST--
Suppresion operator internal functions can return things other than null: true
--EXTENSIONS--
zend_test
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
Exception: Shouldn't be seen
Done
