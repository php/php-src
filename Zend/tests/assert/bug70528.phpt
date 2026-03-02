--TEST--
Bug #70528 (assert() with instanceof adds apostrophes around class name)
--INI--
zend.assertions=1
--FILE--
<?php

namespace Foo;
class Bar {}

$bar = "Bar";
try {
    assert(new \stdClass instanceof $bar);
} catch (\AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
try {
    assert(new \stdClass instanceof Bar);
} catch (\AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
try {
    assert(new \stdClass instanceof \Foo\Bar);
} catch (\AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
?>
--EXPECT--
assert(): assert(new \stdClass() instanceof $bar) failed
assert(): assert(new \stdClass() instanceof Bar) failed
assert(): assert(new \stdClass() instanceof \Foo\Bar) failed
