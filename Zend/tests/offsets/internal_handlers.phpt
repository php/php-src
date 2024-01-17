--TEST--
Internal handlers
--EXTENSIONS--
zend_test
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

echo 'read op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$r = $o['foo'];
exportObject($o);

echo 'write op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$o['foo'] = true;
exportObject($o);

echo 'read-write op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$o['foo'] += 10;
exportObject($o);

echo 'isset op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$r = isset($o['foo']);
exportObject($o);

echo 'empty op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$r = empty($o['foo']);
exportObject($o);

echo 'null coalescing op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$r = $o['foo'] ?? 'default';
exportObject($o);

echo 'appending op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$o[] = true;
exportObject($o);

echo 'unset op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
unset($o['foo']);
exportObject($o);

echo 'nested read', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$r = $o['foo']['bar'];
exportObject($o);

// Illegal
//echo 'nested read: appending then read', PHP_EOL;
//$o = new DimensionHandlersNoArrayAccess();
//try {
//    $r = $o[]['bar'];
//} catch (\Throwable $e) {
//    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
//}

echo 'nested write', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
try {
    $o['foo']['bar'] = true;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($o);

echo 'nested write: appending then write', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
try {
    $o[]['bar'] = true;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($o);

echo 'nested read-write', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
try {
    $o['foo']['bar'] += 10;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($o);

echo 'nested read-write: appending then write', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
try {
    $o[]['bar'] += 10;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($o);

echo 'nested isset', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
try {
    $r = isset($o['foo']['bar']);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($o);

// Illegal
//echo 'nested isset: appending then read', PHP_EOL;
//try {
//    $o = new DimensionHandlersNoArrayAccess();
//    $r = isset($o[]['bar']);
//} catch (\Throwable $e) {
//    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
//}
//exportObject($o);

echo 'nested empty', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
try {
    $r = empty($o['foo']['bar']);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($o);

// Illegal
//echo 'nested empty: appending then read', PHP_EOL;
//try {
//    $o = new DimensionHandlersNoArrayAccess();
//    $r = empty($o[]['bar']);
//} catch (\Throwable $e) {
//    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
//}
//exportObject($o);

echo 'nested null coalescing', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
try {
    $r = $o['foo']['bar'] ?? 'default';
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($o);

// Illegal
//echo 'nested null coalescing: appending then read', PHP_EOL;
//try {
//    $o = new DimensionHandlersNoArrayAccess();
//    $r = $o[]['bar'] ?? 'default';
//} catch (\Throwable $e) {
//    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
//}
//exportObject($o);

echo 'nested appending', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
try {
    $o['foo'][] = true;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($o);

echo 'nested appending: appending then append', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
try {
    $o[][] = true;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($o);

echo 'nested unset', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
try {
    unset($o['foo']['bar']);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($o);

// Illegal
//echo 'nested unset: appending then read', PHP_EOL;
//try {
//    $o = new DimensionHandlersNoArrayAccess();
//    unset($o[]['bar']);
//} catch (\Throwable $e) {
//    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
//}
//exportObject($o);

?>
--EXPECTF--
read op
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_R, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
write op
DimensionHandlersNoArrayAccess, read: false, write: true, has: false, unset: false, readType: uninitialized, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
read-write op
DimensionHandlersNoArrayAccess, read: true, write: true, has: false, unset: false, readType: BP_VAR_R, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
isset op
DimensionHandlersNoArrayAccess, read: false, write: false, has: true, unset: false, readType: uninitialized, hasOffset: true, checkEmpty: 0, offset: 'foo'
empty op
DimensionHandlersNoArrayAccess, read: false, write: false, has: true, unset: false, readType: uninitialized, hasOffset: true, checkEmpty: 1, offset: 'foo'
null coalescing op
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_IS, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
appending op
DimensionHandlersNoArrayAccess, read: false, write: true, has: false, unset: false, readType: uninitialized, hasOffset: false, checkEmpty: uninitialized, offset: uninitialized
unset op
DimensionHandlersNoArrayAccess, read: false, write: false, has: false, unset: true, readType: uninitialized, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
nested read

Warning: Trying to access array offset on true in %s on line %d
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_R, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
nested write

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot use a scalar value as an array
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_W, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
nested write: appending then write

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot use a scalar value as an array
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_W, hasOffset: false, checkEmpty: uninitialized, offset: uninitialized
nested read-write

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot use a scalar value as an array
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_RW, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
nested read-write: appending then write

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot use a scalar value as an array
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_RW, hasOffset: false, checkEmpty: uninitialized, offset: uninitialized
nested isset
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_IS, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
nested empty
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_IS, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
nested null coalescing
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_IS, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
nested appending

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot use a scalar value as an array
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_W, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
nested appending: appending then append

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot use a scalar value as an array
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_W, hasOffset: false, checkEmpty: uninitialized, offset: uninitialized
nested unset

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot unset offset in a non-array variable
DimensionHandlersNoArrayAccess, read: true, write: false, has: false, unset: false, readType: BP_VAR_UNSET, hasOffset: true, checkEmpty: uninitialized, offset: 'foo'
