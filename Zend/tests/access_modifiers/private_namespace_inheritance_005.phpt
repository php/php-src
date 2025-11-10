--TEST--
private(namespace) methods require compatible signature when redeclared in different namespace
--FILE--
<?php

namespace App\Auth {
    class Base {
        private(namespace) function helper(string $arg): void {
            echo "Base: $arg\n";
        }
    }
}

namespace App\Other {
    class Child extends \App\Auth\Base {
        // Incompatible signature - should cause error
        private(namespace) function helper(int $arg): void {
            echo "Child: $arg\n";
        }
    }
}

?>
--EXPECTF--
Fatal error: Declaration of App\Other\Child::helper(int $arg): void must be compatible with App\Auth\Base::helper(string $arg): void in %s on line %d
