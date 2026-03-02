--TEST--
#[Override] attribute in trait does not check for parent class implementations
--FILE--
<?php

class A {
    public function foo(): void {}
}

interface I {
    public function foo(): void;
}

trait T {
    #[\Override]
    public function foo(): void {
        echo 'foo';
    }
}

// Works fine
class B implements I {
    use T;
}

// Works fine ("copied and pasted into the target class")
class C extends A {
    #[\Override]
    public function foo(): void {
        echo 'foo';
    }
}

// Does not work
class D extends A {
    use T;
}
echo "Done";

?>
--EXPECT--
Done
