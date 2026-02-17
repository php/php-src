--TEST--
Invalid trait insteadof list causing mutual exclusion
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

class C {
    use A, B {
        A::test insteadof B;
        B::test insteadof A;
    }
}

$c = new C();
var_dump($c->test());
?>
--EXPECTF--
Fatal error: Invalid trait method precedence for test() - all implementations have been excluded by insteadof rules in %s on line %d
