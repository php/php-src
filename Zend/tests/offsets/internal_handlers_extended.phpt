--TEST--
Internal handlers that do not explicitly support userland ArrayAccess
--EXTENSIONS--
zend_test
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

class NoImplement extends DimensionHandlersNoArrayAccess {}

class DoImplement extends DimensionHandlersNoArrayAccess implements ArrayAccess {
    public function offsetSet($offset, $value): void {
        var_dump(__METHOD__);
        var_dump($offset);
        var_dump($value);
    }
    public function offsetGet($offset): mixed {
        var_dump(__METHOD__);
        var_dump($offset);
        return "dummy";
    }
    public function offsetUnset($offset): void {
        var_dump(__METHOD__);
        var_dump($offset);
    }
    public function offsetExists($offset): bool {
        var_dump(__METHOD__);
        var_dump($offset);
        return true;
    }
}

$no = new NoImplement();
$do = new DoImplement();

try {
    $no['foo'];
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($no);

try {
    $do['foo'];
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
exportObject($do);

?>
--EXPECT--
Error: Cannot use object of type NoImplement as array
NoImplement, read: false, write: false, has: false, unset: false, readType: uninitialized, hasOffset: false, checkEmpty: uninitialized, offset: uninitialized
string(22) "DoImplement::offsetGet"
string(3) "foo"
DoImplement, read: false, write: false, has: false, unset: false, readType: uninitialized, hasOffset: false, checkEmpty: uninitialized, offset: uninitialized
