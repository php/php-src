--TEST--
OOM: debug_backtrace() doesn't reach hard limit - args
--INI--
memory_limit=8M
--FILE--
<?php

function f(int $n) {
    if ($n > 0) {
        f($n - 1, ...$GLOBALS['args']);
        return;
    }
    debug_backtrace();
}

$args = array_fill(0, 512, 1);
f(680, ...$args);

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
