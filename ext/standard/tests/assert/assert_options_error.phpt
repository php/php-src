--TEST--
assert_options() - unknown assert option.

--FILE--
<?php
try {
    assert_options(1000);
} catch (\ValueError $e) {
    echo $e->getMessage();
}
?>
--EXPECTF--
Deprecated: Function assert_options() is deprecated in %s on line %d
assert_options(): Argument #1 ($option) must be an ASSERT_* constant
