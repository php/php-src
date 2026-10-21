--TEST--
OOM: Exception constructor doesn't reach hard limit while fetching backtrace
--INI--
memory_limit=8M
zend.exception_ignore_args=0
--FILE--
<?php

function f(int $n) {
    if ($n > 0) {
        f($n - 1);
        return;
    }
    throw new Exception();
}

try {
    f(30000);
} catch (Exception $e) {
}

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
