--TEST--
Bug #55355 (Abstract functions required by a trait are not correctly found when implemented in an ancestor class)
--FILE--
<?php

// A trait that has a abstract function
trait ATrait {
    function bar() {
        $this->foo();
    }
    abstract function foo();
}

// A class on the second level in the
// inheritance chain
class Level2Impl {
    function foo() {}
}

class Level1Indirect extends Level2Impl {}

// A class on the first level in the
// inheritance chain
class Level1Direct {
    function foo() {}
}

// Trait Uses

class Direct {
    use ATrait;
    function foo() {}
}

class BaseL2 extends Level1Indirect {
    use ATrait;
}

class BaseL1 extends Level1Direct {
    use ATrait;
}

echo 'DONE';
?>
--EXPECT--
DONE
