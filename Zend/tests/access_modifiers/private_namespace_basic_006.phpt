--TEST--
private(namespace) property access from different namespace fails
--FILE--
<?php

namespace App\Models {
    class User {
        private(namespace) int $id = 42;
    }
}

namespace App\Controllers {
    class UserController {
        public function showUserId(): void {
            $user = new \App\Models\User();
            echo $user->id;
        }
    }

    $controller = new UserController();
    $controller->showUserId();
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access private(namespace) property App\Models\User::$id from scope App\Controllers\UserController in %s:%d
Stack trace:
#0 %s(%d): App\Controllers\UserController->showUserId()
#1 {main}
  thrown in %s on line %d
