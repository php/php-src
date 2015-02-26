--TEST--
Visibility modifiers: Namespaced private class, top level instantiation
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace
{
    $b = new A\B();
}

?>
--EXPECTF--
Fatal error: Top level scope cannot instantiate package private class A\B in %s on line %d
