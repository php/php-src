--TEST--
Local function which falls back to global is NOT aliased to the global one, dynamic string
--FILE--
<?php

namespace Foo {
     var_dump(strlen('hello')); // triggers name pinning
}
namespace Bar {
    $v = 'Foo\strlen';
    var_dump($v('hello'));
}

?>
--EXPECTF--
int(5)

Fatal error: Uncaught Error: Call to undefined function Foo\strlen() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
