--TEST--
test catching failed assertion
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php
try {
    assert(false);
} catch (AssertionError $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECT--
AssertionError: assert(false)
