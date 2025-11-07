--TEST--
private(namespace) method access from same namespace (different class)
--FILE--
<?php

namespace App\Services {
    class UserService {
        private(namespace) function validate(): string {
            return "validated";
        }
    }

    class AuthService {
        public function authenticate(): string {
            $service = new UserService();
            return $service->validate();
        }
    }

    $auth = new AuthService();
    echo $auth->authenticate() . "\n";
}

?>
--EXPECT--
validated
