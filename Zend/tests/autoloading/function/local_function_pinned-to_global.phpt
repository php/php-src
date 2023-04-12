--TEST--
Local function which falls back to global is aliased to the global one
--FILE--
<?php

namespace Bar {
    if ( function_exists('Foo\strlen') ) {
        var_dump(\Foo\strlen('hello'));
    }
}
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
\Foo\strlen() was bound to global \strlen()
int(5)
