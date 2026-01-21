--TEST--
Promoted readonly property reassignment in constructor - validation
--FILE--
<?php

class User {
    public function __construct(
        public readonly string $email,
    ) {
        // Simple validation without filter extension
        if (!str_contains($email, '@')) {
            throw new InvalidArgumentException('Invalid email');
        }
        $this->email = strtolower($email);  // Normalize
    }
}

$user = new User('TEST@Example.COM');
var_dump($user->email);

try {
    new User('not-an-email');
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(16) "test@example.com"
Invalid email
