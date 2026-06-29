--TEST--
Bug #70228 (memleak if return in finally block)
--FILE--
<?php

function foo() {
    try { return str_repeat("a", 2); }
    finally { return str_repeat("b", 2); }
}

var_dump(foo());
?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
string(2) "bb"