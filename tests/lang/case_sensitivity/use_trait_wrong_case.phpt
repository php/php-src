--TEST--
Using a trait with wrong-case name fails with wrong case
--FILE--
<?php
trait MyTrait {
    public function hello(): string {
        return "hello";
    }
}

class Child {
    use MYTRAIT;
}

$obj = new Child();
echo $obj->hello() . "\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Trait "MYTRAIT" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
