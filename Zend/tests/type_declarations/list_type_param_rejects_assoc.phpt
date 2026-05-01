--TEST--
list<T> parameter rejects associative array
--FILE--
<?php
class User {}

function f(list<User> $users): void {
    echo "inside\n";
}

try {
    f(['x' => new User]);
} catch (TypeError $e) {
    echo "TypeError\n";
}
?>
--EXPECT--
TypeError
