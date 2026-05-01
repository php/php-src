--TEST--
list<T> parameter rejects non-matching object element
--FILE--
<?php
class User {}

function f(list<User> $users): void {
    echo "inside\n";
}

try {
    f([new stdClass]);
} catch (TypeError $e) {
    echo "TypeError\n";
}
?>
--EXPECT--
TypeError
