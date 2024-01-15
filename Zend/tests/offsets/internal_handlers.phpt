--TEST--
Internal handlers
--EXTENSIONS--
zend_test
--FILE--
<?php

// require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

echo 'read op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$r = $o['foo'];
var_dump($o);

echo 'write op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$o['foo'] = true;
var_dump($o);

echo 'isset op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$r = isset($o['foo']);
var_dump($o);

echo 'empty op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$r = empty($o['foo']);
var_dump($o);

echo 'null coalescing op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$r = $o['foo'] ?? 'default';
var_dump($o);

echo 'appending op', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$o[] = true;
var_dump($o);

echo 'nested read', PHP_EOL;
$o = new DimensionHandlersNoArrayAccess();
$r = $o['foo']['bar'];
var_dump($o);

// Illegal
//echo 'nested read: appending then read', PHP_EOL;
//$o = new DimensionHandlersNoArrayAccess();
//$r = $o[]['bar'];
//var_dump($o);

echo 'nested write', PHP_EOL;
try {
    $o = new DimensionHandlersNoArrayAccess();
    $o['foo']['bar'] = true;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($o);

echo 'nested write: appending then write', PHP_EOL;
try {
    $o = new DimensionHandlersNoArrayAccess();
    $o[]['bar'] = true;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($o);

echo 'nested isset', PHP_EOL;
try {
    $o = new DimensionHandlersNoArrayAccess();
    $r = isset($o['foo']['bar']);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($o);

// Illegal
//echo 'nested isset: appending then read', PHP_EOL;
//try {
//    $o = new DimensionHandlersNoArrayAccess();
//    $r = isset($o[]['bar']);
//    var_dump($o);
//} catch (\Throwable $e) {
//    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
//}

echo 'nested empty', PHP_EOL;
try {
    $o = new DimensionHandlersNoArrayAccess();
    $r = empty($o['foo']['bar']);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($o);

// Illegal
//echo 'nested empty: appending then read', PHP_EOL;
//try {
//    $o = new DimensionHandlersNoArrayAccess();
//    $r = empty($o[]['bar']);
//    var_dump($o);
//} catch (\Throwable $e) {
//    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
//}

echo 'nested null coalescing', PHP_EOL;
try {
    $o = new DimensionHandlersNoArrayAccess();
    $r = $o['foo']['bar'] ?? 'default';
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($o);

// Illegal
//echo 'nested null coalescing: appending then read', PHP_EOL;
//try {
//    $o = new DimensionHandlersNoArrayAccess();
//    $r = $o[]['bar'] ?? 'default';
//    var_dump($o);
//} catch (\Throwable $e) {
//    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
//}

echo 'nested appending', PHP_EOL;
try {
    $o = new DimensionHandlersNoArrayAccess();
    $o['foo'][] = true;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($o);

echo 'nested appending: appending then append', PHP_EOL;
try {
    $o = new DimensionHandlersNoArrayAccess();
    $o[][] = true;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($o);

?>
--EXPECTF--
read op
object(DimensionHandlersNoArrayAccess)#%d (7) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(0)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
write op
object(DimensionHandlersNoArrayAccess)#%d (6) {
  ["read"]=>
  bool(false)
  ["write"]=>
  bool(true)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  uninitialized(int)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
isset op
object(DimensionHandlersNoArrayAccess)#%d (7) {
  ["read"]=>
  bool(false)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(true)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  uninitialized(int)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  int(0)
  ["offset"]=>
  string(3) "foo"
}
empty op
object(DimensionHandlersNoArrayAccess)#%d (7) {
  ["read"]=>
  bool(false)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(true)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  uninitialized(int)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  int(1)
  ["offset"]=>
  string(3) "foo"
}
null coalescing op
object(DimensionHandlersNoArrayAccess)#%d (7) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(3)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
appending op
object(DimensionHandlersNoArrayAccess)#%d (5) {
  ["read"]=>
  bool(false)
  ["write"]=>
  bool(true)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  uninitialized(int)
  ["hasOffset"]=>
  bool(false)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  uninitialized(mixed)
}
nested read

Warning: Trying to access array offset on true in %s on line %d
object(DimensionHandlersNoArrayAccess)#%d (7) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(0)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
nested write

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot use a scalar value as an array
object(DimensionHandlersNoArrayAccess)#%d (7) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(1)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
nested write: appending then write

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot use a scalar value as an array
object(DimensionHandlersNoArrayAccess)#%d (6) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(1)
  ["hasOffset"]=>
  bool(false)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  uninitialized(mixed)
}
nested isset
object(DimensionHandlersNoArrayAccess)#%d (7) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(3)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
nested empty
object(DimensionHandlersNoArrayAccess)#%d (7) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(3)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
nested null coalescing
object(DimensionHandlersNoArrayAccess)#%d (7) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(3)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
nested appending

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot use a scalar value as an array
object(DimensionHandlersNoArrayAccess)#%d (7) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(1)
  ["hasOffset"]=>
  bool(true)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  string(3) "foo"
}
nested appending: appending then append

Notice: Indirect modification of overloaded element of DimensionHandlersNoArrayAccess has no effect in %s on line %d
Error: Cannot use a scalar value as an array
object(DimensionHandlersNoArrayAccess)#%d (6) {
  ["read"]=>
  bool(true)
  ["write"]=>
  bool(false)
  ["has"]=>
  bool(false)
  ["unset"]=>
  bool(false)
  ["readType"]=>
  int(1)
  ["hasOffset"]=>
  bool(false)
  ["checkEmpty"]=>
  uninitialized(int)
  ["offset"]=>
  uninitialized(mixed)
}
