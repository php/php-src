--TEST--
final alias - positive test variation
--FILE--
<?php
trait T1 {
    function foo() {
        echo "Done\n";
    }
}
class C1 {
    use T1 {
        T1::foo as final;
    }
}
class C2 extends C1 {
    public function bar() {}
}
(new C2)->foo();
?>
--EXPECT--
Done
