--TEST--
catch without capturing a variable - exception in destructor
--FILE--
<?php
class ThrowsOnDestruct extends Exception {
    public function __destruct() {
        echo "Throwing\n";
        throw new RuntimeException(__METHOD__);
    }
}
try {
    throw new ThrowsOnDestruct();
} catch (Exception) {
    echo "Unreachable catch\n";
}
echo "Unreachable fallthrough\n";

--EXPECT--
Throwing
