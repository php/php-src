--TEST--
assert_options() - unknown assert option.

--FILE--
<?php
try {
    assert_options(1000);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Deprecated: Function assert_options() is deprecated since 8.3 in %s on line %d
ValueError: assert_options(): Argument #1 ($option) must be an ASSERT_* constant
