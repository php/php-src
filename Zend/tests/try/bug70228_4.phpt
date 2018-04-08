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
                try {
                } finally {
                    return 42;
                }
            } finally {
                throw new Exception(3);
            }
        } catch (Exception $e) {}
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
--EXPECT--
1
