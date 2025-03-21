--TEST--
constructors
--FILE--
<?php

class User {
    public private(set) string $name;
    public private(set) string $email;

    private function __construct(self:>Builder $builder) {
        $this->name = $builder->name;
        $this->email = $builder->email;
    }

    public readonly final class Builder {
        public function __construct(public private(set) string|null $name = null, public private(set) string|null $email = null) {}

        public function withEmail(string $email): self {
            return new self($this->name, $email);
        }

        public function withName(string $name): self {
            return new self($name, $this->email);
        }

        public function build(): User {
            return new User($this);
        }
    }
}

$user = new User:>Builder()->withName('Rob')->withEmail('rob@example.com')->build();
var_dump($user);
?>
--EXPECT--
object(User)#2 (2) {
  ["name"]=>
  string(3) "Rob"
  ["email"]=>
  string(15) "rob@example.com"
}
