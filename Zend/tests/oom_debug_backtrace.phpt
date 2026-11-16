--TEST--
OOM: debug_backtrace() doesn't reach hard limit
--INI--
memory_limit=8M
--FILE--
<?php

function f(int $n) {
    if ($n > 0) {
        f($n - 1);
        return;
    }
    debug_backtrace();
}

f(30000);

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
