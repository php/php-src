--TEST--
#[Override] attribute in trait does not check for parent class implementations (Variant with private parent method)
--FILE--
<?php

class A {
    private function foo(): void {}
}

trait T {
    #[\Override]
    public function foo(): void {
        echo 'foo';
    }
}

class D extends A {
    use T;
}
echo "Done";

?>
--EXPECTF--
Fatal error: D::foo() has #[\Override] attribute, but no matching parent method exists in %s on line %d
