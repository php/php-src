--TEST--
Bug #70228 (memleak if return in finally block)
--FILE--
<?php
function test() {
    try {
        throw new Exception(1);
    } finally {
        try {
            try {
            } finally {
                return 42;
            }
        } finally {
            throw new Exception(2);
        }
    }
}

try {
    var_dump(test());
} catch (Throwable $e) {
    do {
        echo $e::class, ': ', $e->getMessage(), "\n";
        $e = $e->getPrevious();
    } while ($e);
}
?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
Exception: 2
Exception: 1
