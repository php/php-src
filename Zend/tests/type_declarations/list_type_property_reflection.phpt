--TEST--
list<T> property reflection string
--FILE--
<?php
class User {}

class Box {
    public list<User> $users;
}

$prop = new ReflectionProperty(Box::class, 'users');
echo (string) $prop->getType(), "\n";
?>
--EXPECT--
list<User>
