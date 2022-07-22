--TEST--
Test that internal functions can accept intersection types via ZPP
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


try {
    zend_intersection_type(new EmptyIterator());
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
try {
    zend_intersection_type(new C());
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

zend_intersection_type(new I());

?>
==DONE==
--EXPECT--
zend_intersection_type(): Argument #1 ($v) must be of type Traversable&Countable, EmptyIterator given
zend_intersection_type(): Argument #1 ($v) must be of type Traversable&Countable, C given
==DONE==
