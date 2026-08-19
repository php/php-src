--TEST--
Bug #72213 (Finally leaks on nested exceptions)
--FILE--
<?php
function test() {
    try {
        throw new Exception(1);
    } finally {
        try {
            try {
                throw new Exception(2);
            } finally {
            }
        } catch (Exception $e) {
        }
    }
}

try {
    test();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: 1
