--TEST--
noreturn return type: unacceptable cases: any return
--FILE--
<?php

function foo(): noreturn {
    return "hello"; // not permitted in a noreturn function
}

// Note the lack of function call: function validated at compile-time
?>
--EXPECTF--
Fatal error: A noreturn function must not return in %s on line %d
