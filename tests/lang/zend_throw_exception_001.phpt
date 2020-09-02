--TEST--
zend_throw_exception with NULL message
--FILE--
<?php
assert_options(ASSERT_EXCEPTION, true);
try {
    $assert = 'assert';
    $assert(false);
} catch (AssertionError $assertionError) {
    echo 'Done' . PHP_EOL;
}
?>
--EXPECT--
Done
