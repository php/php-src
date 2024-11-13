--TEST--
#[Override] attribute in trait does not check for parent class implementations (Variant with protected parent method)
--FILE--
<?php

class A {
    protected function foo(): void {}
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
Done
