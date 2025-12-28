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
    echo $e->getMessage(), "\n";
}
try {
    _ZendTestClass::test('a', 'b', c: 'c');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Unknown named parameter $c
Unknown named parameter $c
