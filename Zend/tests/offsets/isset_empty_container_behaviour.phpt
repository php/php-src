--TEST--
Behaviour of isset()/empty() with various container types
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

foreach ($allContainers as $container) {
    echo zend_test_var_export($container), '[0]', "\n";

    try {
        var_dump(isset($container[0]));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    try {
        var_dump(empty($container[0]));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
NULL[0]
bool(false)
bool(true)
false[0]
bool(false)
bool(true)
true[0]
bool(false)
bool(true)
4[0]
bool(false)
bool(true)
5.5[0]
bool(false)
bool(true)
'10'[0]
bool(true)
bool(false)
'25.5'[0]
bool(true)
bool(false)
'string'[0]
bool(true)
bool(false)
[][0]
bool(false)
bool(true)
STDERR[0]
bool(false)
bool(true)
new stdClass()[0]
Error: Cannot use object of type stdClass as array
Error: Cannot use object of type stdClass as array
new ArrayObject()[0]
bool(false)
bool(true)
new A()[0]
string(15) "A::offsetExists"
int(0)
string(12) "A::offsetGet"
int(0)
bool(true)
string(15) "A::offsetExists"
int(0)
string(12) "A::offsetGet"
int(0)
bool(false)
new B()[0]
string(15) "B::offsetExists"
int(0)
string(12) "B::offsetGet"
int(0)
bool(true)
string(15) "B::offsetExists"
int(0)
string(12) "B::offsetGet"
int(0)
bool(false)
new DimensionRead()[0]
string(27) "DimensionRead::offsetExists"
int(0)
string(24) "DimensionRead::offsetGet"
int(0)
bool(true)
string(27) "DimensionRead::offsetExists"
int(0)
string(24) "DimensionRead::offsetGet"
int(0)
bool(false)
new DimensionFetch()[0]
string(27) "DimensionRead::offsetExists"
int(0)
string(24) "DimensionRead::offsetGet"
int(0)
bool(true)
string(27) "DimensionRead::offsetExists"
int(0)
string(24) "DimensionRead::offsetGet"
int(0)
bool(false)
new DimensionWrite()[0]
Error: Cannot read offset of object of type DimensionWrite
Error: Cannot read offset of object of type DimensionWrite
new DimensionReadWrite()[0]
string(32) "DimensionReadWrite::offsetExists"
int(0)
string(29) "DimensionReadWrite::offsetGet"
int(0)
bool(true)
string(32) "DimensionReadWrite::offsetExists"
int(0)
string(29) "DimensionReadWrite::offsetGet"
int(0)
bool(false)
new DimensionAppend()[0]
Error: Cannot read offset of object of type DimensionAppend
Error: Cannot read offset of object of type DimensionAppend
new DimensionFetchAppend()[0]
Error: Cannot read offset of object of type DimensionFetchAppend
Error: Cannot read offset of object of type DimensionFetchAppend
new DimensionUnset()[0]
Error: Cannot read offset of object of type DimensionUnset
Error: Cannot read offset of object of type DimensionUnset
