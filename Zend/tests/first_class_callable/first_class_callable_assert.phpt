--TEST--
Acquire callable to assert()
--FILE--
<?php

namespace Foo;

$assert = assert(...);
$assert(1 == 1.0, "Message 1");
try {
    $assert(1 == 2.0, "Message 2");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    assert(false && strlen(...));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: Message 2
AssertionError: assert(false && strlen(...))
