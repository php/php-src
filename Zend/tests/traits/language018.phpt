--TEST--
abstract alias
--FILE--
<?php
trait T1 {
    function foo() {}
}
class C1 {
    use T1 {
        T1::foo as abstract;
    }
}
?>
--EXPECTF--
Fatal error: Cannot use "abstract" as method modifier in trait alias in %s on line %d
