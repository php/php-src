--TEST--
private(namespace) property access from same namespace (same class)
--FILE--
<?php

namespace App\Models {
    class User {
        private(namespace) int $id = 42;

        public function getId(): int {
            return $this->id;
        }
    }

    $user = new User();
    echo $user->getId() . "\n";
}

?>
--EXPECT--
42
