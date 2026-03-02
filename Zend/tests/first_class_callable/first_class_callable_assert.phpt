--TEST--
Acquire callable to assert()
--FILE--
<?php

namespace Foo;

$assert = assert(...);
$assert(1 == 1.0, "Message 1");
try {
    $assert(1 == 2.0, "Message 2");
} catch (\AssertionError $e) {
    echo $e->getMessage(), "\n";
}

try {
    assert(false && strlen(...));
} catch (\AssertionError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Message 2
assert(false && strlen(...))
