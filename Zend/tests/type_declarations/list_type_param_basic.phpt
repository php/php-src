--TEST--
list<T> parameter accepts matching object list
--FILE--
<?php
class User {}

function f(list<User> $users): void {
    echo count($users), "\n";
}

f([new User, new User]);
?>
--EXPECT--
2
