--TEST--
Don't optimize dynamic call to non-dynamic one if it drops the warning
--FILE--
<?php

function test() {
    try {
        ((string) 'extract')(['a' => 42]);
    } catch (\Error $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
test();

?>
--EXPECT--
Error: Cannot call extract() dynamically
