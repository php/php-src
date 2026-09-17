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
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    assert(new \stdClass instanceof Bar);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    assert(new \stdClass instanceof \Foo\Bar);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
AssertionError: assert(new \stdClass() instanceof $bar)
AssertionError: assert(new \stdClass() instanceof Bar)
AssertionError: assert(new \stdClass() instanceof \Foo\Bar)
