--TEST--
GH-16266 (_ZendTestClass::test() segfaults on named parameter)
--EXTENSIONS--
zend_test
--FILE--
<?php
try {
    $o = new _ZendTestClass();
    $o->test('a', 'b', c: 'c');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    _ZendTestClass::test('a', 'b', c: 'c');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Unknown named parameter $c
Error: Unknown named parameter $c
