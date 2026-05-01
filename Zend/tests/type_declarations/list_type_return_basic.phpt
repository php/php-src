--TEST--
list<T> return type accepts matching object list
--FILE--
<?php
class User {}

function f(): list<User> {
    return [new User, new User];
}

echo count(f()), "\n";
?>
--EXPECT--
2
