--TEST--
never return type: unacceptable cases: empty return in a function
--FILE--
<?php

function foo(): never {
    return; // not permitted in a never function
}

// Note the lack of function call: function validated at compile-time
?>
--EXPECTF--
Fatal error: A never-returning function must not return in %s on line %d
