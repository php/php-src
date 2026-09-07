--TEST--
test providing reason (fail)
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php
try {
    assert(false, "I require this to succeed");
} catch (Throwable $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECT--
AssertionError: I require this to succeed
