--TEST--
OOM: debug_backtrace() doesn't reach hard limit - named args
--INI--
memory_limit=8M
--FILE--
<?php

/* A variadic is required: extra named args are only accepted by a function
 * that collects them. */
function f(int $n, ...$rest) {
    if ($n > 0) {
        f($n - 1, ...$GLOBALS['named']);
        return;
    }
    debug_backtrace();
}

$named = [];
for ($i = 0; $i < 512; $i++) {
    $named["a$i"] = 1;
}
$GLOBALS['named'] = $named;

f(450, ...$named);

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
