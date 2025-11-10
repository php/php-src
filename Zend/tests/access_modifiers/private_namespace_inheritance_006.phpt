--TEST--
private(namespace) methods can be redeclared with compatible signature in different namespace
--FILE--
<?php

namespace App\Auth {
    class Base {
        private(namespace) function helper(string $arg): void {
            echo "Base: $arg\n";
        }

        public function callHelper(): void {
            $this->helper("from base");
        }
    }

    // Child in SAME namespace can properly override
    class ChildSameNamespace extends Base {
        private(namespace) function helper(string $arg): void {
            echo "ChildSameNamespace: $arg\n";
        }
    }
}

namespace App\Other {
    // Compatible signature is accepted (no compile error)
    // But this doesn't create a polymorphic override relationship
    class ChildDifferentNamespace extends \App\Auth\Base {
        private(namespace) function helper(string $arg): void {
            echo "ChildDifferentNamespace: $arg\n";
        }

        public function callChildHelper(): void {
            // This works - calling from App\Other namespace
            $this->helper("from child");
        }
    }

    $child = new ChildDifferentNamespace();
    $child->callChildHelper(); // Works - calling from App\Other
}

namespace App\Auth {
    // Demonstrate that same namespace child DOES override polymorphically
    $child2 = new ChildSameNamespace();
    $child2->callHelper(); // Calls ChildSameNamespace::helper (polymorphic)
}

?>
--EXPECT--
ChildDifferentNamespace: from child
ChildSameNamespace: from base
