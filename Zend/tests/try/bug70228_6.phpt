--TEST--
Bug #70228 (memleak if return in finally block)
--FILE--
<?php
function test($x) {
    foreach ($x as $v) {
        try {
            return str_repeat("a", 2);
        } finally {
            return 42;
        }
    }
}

var_dump(test([1]));
?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
int(42)