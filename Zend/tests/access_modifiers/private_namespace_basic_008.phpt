--TEST--
private(namespace) access from parent namespace fails (exact namespace match required)
--FILE--
<?php

namespace App\Services\Auth {
    class AuthService {
        private(namespace) function authenticate(): string {
            return "authenticated";
        }
    }
}

namespace App\Services {
    class Consumer {
        public function doAuth(): void {
            $auth = new Auth\AuthService();
            $auth->authenticate();
        }
    }

    $consumer = new Consumer();
    $consumer->doAuth();
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private(namespace) method App\Services\Auth\AuthService::authenticate() from scope App\Services\Consumer in %s:%d
Stack trace:
#0 %s(%d): App\Services\Consumer->doAuth()
#1 {main}
  thrown in %s on line %d
