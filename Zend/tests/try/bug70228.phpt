--TEST--
Bug #70228 (memleak if return in finally block)
--XFAIL--
See https://bugs.php.net/bug.php?id=70228
--FILE--
<?php

function foo() {
    try { return str_repeat("a", 2); }
    finally { return true; }
}

var_dump(foo());
?>
--EXPECT--
string(3) "bar"
