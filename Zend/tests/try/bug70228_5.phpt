--TEST--
Bug #70228 (memleak if return hidden by throw in finally block)
--FILE--
<?php
function test() {
    try {
        return str_repeat("a", 2);
    } finally {
        throw new Exception("ops");
    }
}

try {
    test();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
ops
