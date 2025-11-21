--TEST--
private(namespace) property access from same namespace (different class)
--FILE--
<?php

namespace App\Models {
    class User {
        private(namespace) int $id = 42;
    }

    class UserRepository {
        public function getUserId(): int {
            $user = new User();
            return $user->id;
        }
    }

    $repo = new UserRepository();
    echo $repo->getUserId() . "\n";
}

?>
--EXPECT--
42
