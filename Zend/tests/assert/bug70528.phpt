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
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    assert(new \stdClass instanceof Bar);
} catch (\AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    assert(new \stdClass instanceof \Foo\Bar);
} catch (\AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
AssertionError: assert(new \stdClass() instanceof $bar)
AssertionError: assert(new \stdClass() instanceof Bar)
AssertionError: assert(new \stdClass() instanceof \Foo\Bar)
