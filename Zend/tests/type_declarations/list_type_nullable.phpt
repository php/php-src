--TEST--
nullable list<T> parameter accepts null and matching object list
--FILE--
<?php
class User {}

function f(?list<User> $users): void {
    echo $users === null ? "null\n" : count($users) . "\n";
}

f(null);
f([new User]);
?>
--EXPECT--
null
1
