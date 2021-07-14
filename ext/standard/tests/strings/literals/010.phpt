--TEST--
Literal Variables in Namespaces
--FILE--
<?php
namespace Foo {
    class Bar {}
    
    function baz() {}
}

namespace {
    $reflector = new \ReflectionClass(\Foo\Bar::class);

    var_dump(
        $reflector->getName(),
        is_literal($reflector->getName()));

    $reflector = new \ReflectionFunction('\Foo\baz');

    var_dump(
        $reflector->getName(),
        is_literal($reflector->getName())); 
}
?>
--EXPECT--
string(7) "Foo\Bar"
bool(true)
string(7) "Foo\baz"
bool(true)
