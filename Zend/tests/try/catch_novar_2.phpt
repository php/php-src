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
    echo "Caught\n";
}
echo "After try/catch\n";

?>
--EXPECTF--
Caught
After try/catch
Throwing

Fatal error: Uncaught RuntimeException: ThrowsOnDestruct::__destruct in %s:%d
Stack trace:
#0 [internal function]: ThrowsOnDestruct->__destruct()
#1 {main}
  thrown in %s on line %d
