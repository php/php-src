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
class C2 extends C1 {
    public function foo() {}
}
?>
--EXPECTF--
Fatal error: Cannot override final method C1::foo() in %s on line %d
