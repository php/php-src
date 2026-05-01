--TEST--
list<T> return type rejects non-matching object element
--FILE--
<?php
class User {}

function f(): list<User> {
    return [new stdClass];
}

try {
    f();
} catch (TypeError $e) {
    echo "TypeError\n";
}
?>
--EXPECT--
TypeError
