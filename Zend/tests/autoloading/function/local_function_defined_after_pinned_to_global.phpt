--TEST--
Local function must be able to be defined after it got pinned to the global one
--FILE--
<?php

namespace Foo;

var_dump(strlen('hello')); // triggers name pinning
if (!function_exists('Foo\strlen') ) {
    function strlen(string $s) {
        return 42;
    }
   var_dump(\Foo\strlen('hello'));
   var_dump(strlen('hello'));
}


?>
--EXPECT--
int(5)
int(42)
int(42)
