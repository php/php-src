--TEST--
private(namespace) properties not accessible from child class in different namespace
--FILE--
<?php

namespace App {
    class ParentClass {
        private(namespace) int $value = 42;
    }
}

namespace Other {
    class Child extends \App\ParentClass {
        public function getValue(): int {
            return $this->value;
        }
    }

    $child = new Child();
    echo $child->getValue();
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access private(namespace) property Other\Child::$value from scope Other\Child in %s:%d
Stack trace:
#0 %s(%d): Other\Child->getValue()
#1 {main}
  thrown in %s on line %d
