--TEST--
private(namespace) methods not accessible from child class in different namespace
--FILE--
<?php

namespace App {
    class ParentClass {
        private(namespace) function test(): string {
            return "parent";
        }
    }
}

namespace Other {
    class Child extends \App\ParentClass {
        public function callTest(): void {
            $this->test();
        }
    }

    $child = new Child();
    $child->callTest();
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method Other\Child::test() in %s:%d
Stack trace:
#0 %s(%d): Other\Child->callTest()
#1 {main}
  thrown in %s on line %d
