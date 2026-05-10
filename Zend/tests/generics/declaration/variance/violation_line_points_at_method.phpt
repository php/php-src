--TEST--
Variance: a violation inside a method signature reports the method's line, not the class header
--FILE--
<?php
final class LinePointer<+T> {
    public function ok(): T {
        throw new RuntimeException();
    }

    public function bad<O: T>(O $default): T|O {
        return $default;
    }
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line 7
