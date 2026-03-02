--TEST--
never return type: never cannot return from throw expression
--FILE--
<?php

function foo() : never {
    return throw new Exception('bad');
}

// Note the lack of function call: function validated at compile-time
?>
--EXPECTF--
Fatal error: A never-returning function must not return in %s on line %d
