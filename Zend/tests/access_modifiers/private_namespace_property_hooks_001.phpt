--TEST--
private(namespace) property with hooks - same namespace access
--FILE--
<?php

namespace App\Auth;

class SessionManager {
    private(namespace) string $token {
        get => strtoupper($this->token);
        set => strtolower($value);
    }
    
    public function __construct() {
        $this->token = "ABC123";
    }
}

class SessionStore {
    public function test(SessionManager $session): void {
        // Same namespace - should work
        var_dump($session->token);
        $session->token = "XYZ789";
        var_dump($session->token);
    }
}

$store = new SessionStore();
$session = new SessionManager();
$store->test($session);

?>
--EXPECT--
string(6) "ABC123"
string(6) "XYZ789"
