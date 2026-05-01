--TEST--
array parameter type is contravariant with list<T>
--FILE--
<?php
class User {}

class A {
    public function setItems(list<User> $items): void {
        echo "A\n";
    }
}

class B extends A {
    public function setItems(array $items): void {
        echo count($items), "\n";
    }
}

(new B)->setItems([new User]);
?>
--EXPECT--
1
