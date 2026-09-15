--TEST--
GH-17773 (Cannot initialize array element with reference to nullsafe chain)
--FILE--
<?php
class Test {
    public function &getReference() {
        static $value = 42;
        return $value;
    }
}

$test = null;
$array = [&$test?->getReference()];
?>
--EXPECTF--
Fatal error: Cannot take reference of a nullsafe chain in %s on line %d
