--TEST--
Friends: RFC example 2 (User and UserBuilder)
--FILE--
<?php

class User {
    friend UserBuilder;

    public protected(set) ?int $userId = null;
    public protected(set) ?string $username = null;

    // Protected constructor - use the UserBuilder
    protected function __construct() {}
}

class UserBuilder {

    public function newWithId(int $userId): User {
        $u = new User();
        $u->userId = $userId;
        return $u;
    }

    public function newWithName(string $username): User {
        $u = new User();
        $u->username = $username;
        return $u;
    }
}

$builder = new UserBuilder();

$alice = $builder->newWithId(1);
var_dump($alice);

$bob = $builder->newWithName("Bob");
var_dump($bob);

// Manipulation outside of the builder fails
try {
	$bob->userId = 2;
} catch (Error $e) {
	echo $e;
}

?>
--EXPECTF--
object(User)#%d (2) {
  ["userId"]=>
  int(1)
  ["username"]=>
  NULL
}
object(User)#%d (2) {
  ["userId"]=>
  NULL
  ["username"]=>
  string(3) "Bob"
}
Error: Cannot modify protected(set) property User::$userId from global scope in %s:%d
Stack trace:
#0 {main}
