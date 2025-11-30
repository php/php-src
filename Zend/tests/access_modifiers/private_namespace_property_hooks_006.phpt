--TEST--
private(namespace) property with hook - inheritance from different namespace
--FILE--
<?php

namespace App\Auth {
    class Base {
        private(namespace) string $value {
            get => strtoupper($this->value);
        }
        
        public function __construct() {
            $this->value = "base";
        }
        
        public function test(): void {
            // Works - same namespace
            var_dump($this->value);
        }
    }
}

namespace App\Other {
    class Child extends \App\Auth\Base {
        public function tryAccess(): void {
            // Fails - different namespace from where property was declared
            var_dump($this->value);
        }
    }
}

namespace {
    $child = new \App\Other\Child();
    $child->test(); // Parent method works
    $child->tryAccess(); // Child method fails
}

?>
--EXPECTF--
string(4) "BASE"

Fatal error: Uncaught Error: Cannot access private(namespace) property App\Other\Child::$value from scope App\Other\Child in %s:%d
Stack trace:
#0 %s(%d): App\Other\Child->tryAccess()
#1 {main}
  thrown in %s on line %d
