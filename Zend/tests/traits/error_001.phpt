--TEST--
Trying to use instanceof for a method twice
--FILE--
<?php

trait foo {
    public function foo() {
        return 1;
    }
}

trait foo2 {
    public function foo() {
        return 2;
    }
}


class A {
    use foo {
        foo2::foo insteadof foo;
        foo2::foo insteadof foo;
    }
}

?>
--EXPECTF--
Fatal error: Required Trait foo2 wasn't added to A in %s on line %d
