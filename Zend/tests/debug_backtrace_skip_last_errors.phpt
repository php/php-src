--TEST--
debug_backtrace skip_last error
--FILE--
<?php
try {
    debug_backtrace(skip_last: -1);
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}
?>
--EXPECT--
debug_backtrace(): Argument #3 ($skip_last) must be greater than or equal to zero
