--TEST--
private(namespace) property hooks in traits - uses receiver class namespace
--FILE--
<?php

namespace App\Traits {
    trait Timestamped {
        private(namespace) int $timestamp {
            get => $this->timestamp;
            set => time();
        }
    }
}

namespace App\Models {
    use App\Traits\Timestamped;
    
    class User {
        use Timestamped;
    }
    
    class UserRepository {
        public function test(User $user): void {
            // Same namespace as User (App\Models), not trait (App\Traits)
            $user->timestamp = 12345;
            var_dump($user->timestamp > 0);
        }
    }
}

namespace App\Controllers {
    $user = new \App\Models\User();
    $repo = new \App\Models\UserRepository();
    $repo->test($user);
    
    // Different namespace from User - should fail
    var_dump($user->timestamp);
}

?>
--EXPECTF--
bool(true)

Fatal error: Uncaught Error: Cannot access private(namespace) property App\Models\User::$timestamp from scope {main} in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
