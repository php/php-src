--TEST--
Test that internal functions warn on improper intersection types return
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
    var_dump(zend_intersection_type(new EmptyIterator()));
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
try {
    var_dump(zend_intersection_type(new C()));
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
var_dump(zend_intersection_type(new I()));

?>
==DONE==
--EXPECT--
zend_intersection_type(): Argument #1 ($v) must be of type Traversable&Countable, EmptyIterator given
zend_intersection_type(): Argument #1 ($v) must be of type Traversable&Countable, C given
object(I)#2 (0) {
}
==DONE==
