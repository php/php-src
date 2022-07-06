--TEST--
Bug #78833 (Integer overflow in pack causes out-of-bound access)
--FILE--
<?php
try {
    var_dump(pack("E2E2147483647H*", 0x0, 0x0, 0x0));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Type E: too few arguments
