--TEST--
Bug #72213 (Finally leaks on nested exceptions)
--FILE--
<?php
function test() {
    try {
        throw new Exception('a');
    } finally {
        try {
            throw new Exception('b');
        } finally {
        }
    }
}

try {
    test();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->getPrevious()->getMessage());
}
?>
--EXPECT--
Exception: b
string(1) "a"
