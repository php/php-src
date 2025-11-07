--TEST--
private(namespace) method access from different namespace fails
--FILE--
<?php

namespace App\Services {
    class UserService {
        private(namespace) function validate(): string {
            return "validated";
        }
    }
}

namespace App\Controllers {
    class UserController {
        public function process(): void {
            $service = new \App\Services\UserService();
            $service->validate();
        }
    }

    $controller = new UserController();
    $controller->process();
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private(namespace) method App\Services\UserService::validate() from scope App\Controllers\UserController in %s:%d
Stack trace:
#0 %s(%d): App\Controllers\UserController->process()
#1 {main}
  thrown in %s on line %d
