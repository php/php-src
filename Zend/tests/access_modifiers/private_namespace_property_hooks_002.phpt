--TEST--
private(namespace) property with hooks - different namespace fails
--FILE--
<?php

namespace App\Auth {
    class SessionManager {
        private(namespace) string $token {
            get => strtoupper($this->token);
        }
        
        public function __construct() {
            $this->token = "abc123";
        }
    }
}

namespace App\Controllers {
    $session = new \App\Auth\SessionManager();
    // Different namespace - should fail
    var_dump($session->token);
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access private(namespace) property App\Auth\SessionManager::$token from scope {main} in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
