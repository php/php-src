--TEST--
never return type: unacceptable cases: any return
--FILE--
<?php

function foo(): never {
    return "hello"; // not permitted in a never function
}

// Note the lack of function call: function validated at compile-time
?>
--EXPECTF--
Fatal error: A never-returning function must not return in %s on line %d
