--TEST--
Friends: RFC example 1 (User and UserFactory)
--FILE--
<?php

class User {
    friend UserFactory;

    // Private constructor - user information must come from a trusted source
    private function __construct(
        public readonly int $userId,
        public readonly string $username,
    ) {}
}

class UserFactory {

    public function newFromId(int $userId): ?User {
        // In reality this would query a database or something
        return match ($userId) {
            1 => new User(1, "Alice"),
            2 => new User(2, "Bob"),
            default => null,
        };
    }
}

$factory = new UserFactory;

$alice = $factory->newFromId(1);
var_dump($alice);

$bob = $factory->newFromId(2);
var_dump($bob);

// Creation outside of the factory fails
try {
	$unknown = new User(3, "Camille");
} catch (Error $e) {
	echo $e;
}

?>
--EXPECTF--
object(User)#%d (2) {
  ["userId"]=>
  int(1)
  ["username"]=>
  string(5) "Alice"
}
object(User)#%d (2) {
  ["userId"]=>
  int(2)
  ["username"]=>
  string(3) "Bob"
}
Error: Call to private User::__construct() from global scope in %s:%d
Stack trace:
#0 {main}
