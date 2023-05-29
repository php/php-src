--TEST--
Test that clone initializer list is displayed properly in assert()
--INI--
zend.assertions = 1
assert.exception = 1
--FILE--
<?php

try {
    assert(clone new stdClass() === null);
} catch (AssertionError $e) {
    echo $e->getMessage() . "\n";
}

try {
    assert(clone new stdClass() with [] === null);
} catch (AssertionError $e) {
    echo $e->getMessage() . "\n";
}

try {
    assert(clone new stdClass() with ["foo" => 1, "bar" => "abc"] === null);
} catch (AssertionError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
assert(clone new stdClass() === null)
assert(clone new stdClass() with [] === null)
assert(clone new stdClass() with ['foo' => 1, 'bar' => 'abc'] === null)
