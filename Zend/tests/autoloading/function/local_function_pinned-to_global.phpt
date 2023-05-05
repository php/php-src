--TEST--
Local function which falls back to global is NOT aliased to the global one, function_exists
--FILE--
<?php

namespace Foo {
     var_dump(strlen('hello')); // triggers name pinning
}
namespace Bar {
    if ( function_exists('Foo\strlen') ) {
        echo '\Foo\strlen() was bound to global \strlen()', \PHP_EOL;
        var_dump(\Foo\strlen('hello'));
    }
}

?>
--EXPECT--
int(5)
