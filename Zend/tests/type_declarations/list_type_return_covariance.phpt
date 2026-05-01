--TEST--
list<T> return type is covariant with array
--FILE--
<?php
class User {}

class A {
    public function getItems(): array {
        return [];
    }
}

class B extends A {
    public function getItems(): list<User> {
        return [new User];
    }
}

echo count((new B)->getItems()), "\n";
?>
--EXPECT--
1
