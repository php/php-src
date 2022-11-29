--TEST--
Test that internal classes can register intersection types
--EXTENSIONS--
zend_test
spl
--FILE--
<?php

class C implements Countable {
    public function count(): int {
        return 1;
    }
}

class I extends EmptyIterator implements Countable {
    public function count(): int {
        return 1;
    }
}

$o = new _ZendTestClass();

try {
    var_dump($o->classIntersectionProp);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o->classIntersectionProp = new EmptyIterator();
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
try {
    $o->classIntersectionProp = new C();
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
$o->classIntersectionProp = new I();

?>
==DONE==
--EXPECT--
Error: Typed property _ZendTestClass::$classIntersectionProp must not be accessed before initialization
Cannot assign EmptyIterator to property _ZendTestClass::$classIntersectionProp of type Traversable&Countable
Cannot assign C to property _ZendTestClass::$classIntersectionProp of type Traversable&Countable
==DONE==
