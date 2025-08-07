--TEST--
Invalid trait insteadof list causing circular exclusion with three traits
--FILE--
<?php

trait A {
    public function test() {
        return 'A';
    }
}

trait B {
    public function test() {
        return 'B';
    }
}

trait C {
    public function test() {
        return 'C';
    }
}

class D {
    use A, B, C {
        A::test insteadof B;
        B::test insteadof C;  
        C::test insteadof A;
    }
}

$d = new D();
var_dump($d->test());
?>
--EXPECTF--
Fatal error: Invalid trait method precedence for test() - all implementations have been excluded by insteadof rules in %s on line %d
