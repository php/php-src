--TEST--
Bug #78833 (Integer overflow in pack causes out-of-bound access)
--FILE--
<?php
try {
    var_dump(pack("E2E2147483647H*", 0x0, 0x0, 0x0));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: Type E: too few arguments
