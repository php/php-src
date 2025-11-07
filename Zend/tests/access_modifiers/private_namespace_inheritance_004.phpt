--TEST--
Cannot reduce visibility from public to private(namespace) in child class
--FILE--
<?php

namespace App {
    class ParentClass {
        public function test(): void {}
    }

    class Child extends ParentClass {
        private(namespace) function test(): void {}
    }
}

?>
--EXPECTF--
Fatal error: Access level to App\Child::test() must not be weaker than App\ParentClass::test() in %s on line %d
