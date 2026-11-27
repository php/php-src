--TEST--
Friends: RFC example 3 (UserBuilder and LoggedUserBuilder)
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

class LoggedUserBuilder extends UserBuilder {

    public function newWithId(int $userId): User {
        echo "Creating user with id: $userId\n";
        return parent::newWithId($userId);
    }

    public function newWithName(string $username): User {
        echo "Creating user with name: $username\n";
        return parent::newWithName($username);
    }
}

$builder = new UserBuilder();

$alice = $builder->newWithId(1);
var_dump($alice);

$bob = $builder->newWithName("Bob");
var_dump($bob);

echo "\n\n----------\n\n";

// LoggedUserBuilder doesn't actually access anything protected, that is all
// done in the parent:: calls, this works fine
$loggedBuilder = new LoggedUserBuilder();
$charlie = $loggedBuilder->newWithId(3);
var_dump($charlie);

$daniel = $loggedBuilder->newWithName("Daniel");
var_dump($daniel);

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


----------

Creating user with id: 3
object(User)#%d (2) {
  ["userId"]=>
  int(3)
  ["username"]=>
  NULL
}
Creating user with name: Daniel
object(User)#%d (2) {
  ["userId"]=>
  NULL
  ["username"]=>
  string(6) "Daniel"
}
