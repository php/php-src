--TEST--
never type: type error on direct call
--FILE--
<?php

function foo(never $var) {
    echo "bad";
}

try {
    foo("value");
} catch (TypeError) {
    echo "type error";
}
?>
--EXPECT--
type error
