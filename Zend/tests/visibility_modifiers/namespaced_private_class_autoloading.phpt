--TEST--
Visibility modifiers: Namespaced private class, autoloading
--FILE--
<?php

spl_autoload_register(function () { eval('namespace A { private class B {} }'); });

$b = new \A\B();

?>
--EXPECTF--
Fatal error: Top level scope cannot instantiate package private class A\B in %s on line %d
