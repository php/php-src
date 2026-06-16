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
} catch (Exception $e) {
    do {
        echo $e->getMessage() . "\n";
        $e = $e->getPrevious();
    } while ($e);
}
?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
2
1