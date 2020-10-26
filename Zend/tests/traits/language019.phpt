--TEST--
final alias
--FILE--
<?php
trait T1 {
    function foo() {}
}
class C1 {
    use T1 {
        T1::foo as final;
    }
}
?>
--EXPECTF--
Fatal error: Cannot use "final" as a trait method alias in %s on line %d
