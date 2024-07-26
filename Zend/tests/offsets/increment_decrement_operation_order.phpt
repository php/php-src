--TEST--
Behaviour of increment/decrement with various container types
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

foreach ($allContainers as $container) {
    echo zend_test_var_export($container), '[0]', "\n";

    try {
        var_dump($container[0]++);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    try {
        var_dump($container[0]--);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECTF--
NULL[0]

Warning: Undefined array key 0 in %s on line %d
NULL
int(1)
false[0]

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d

Warning: Undefined array key 0 in %s on line %d
NULL
int(1)
true[0]
Error: Cannot use a scalar value as an array
Error: Cannot use a scalar value as an array
4[0]
Error: Cannot use a scalar value as an array
Error: Cannot use a scalar value as an array
5.5[0]
Error: Cannot use a scalar value as an array
Error: Cannot use a scalar value as an array
'10'[0]
Error: Cannot increment/decrement string offsets
Error: Cannot increment/decrement string offsets
'25.5'[0]
Error: Cannot increment/decrement string offsets
Error: Cannot increment/decrement string offsets
'string'[0]
Error: Cannot increment/decrement string offsets
Error: Cannot increment/decrement string offsets
[][0]

Warning: Undefined array key 0 in %s on line %d
NULL
int(1)
STDERR[0]
Error: Cannot use a scalar value as an array
Error: Cannot use a scalar value as an array
new stdClass()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
new ArrayObject()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
new A()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
new B()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
new DimensionRead()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
new DimensionFetch()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
new DimensionWrite()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
new DimensionReadWrite()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
new DimensionAppend()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
new DimensionFetchAppend()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
new DimensionUnset()[0]
Error: Cannot increment/decrement object offsets
Error: Cannot increment/decrement object offsets
