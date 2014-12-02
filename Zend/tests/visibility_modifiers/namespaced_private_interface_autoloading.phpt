--TEST--
Visibility modifiers: Namespaced private interface, autoloading
--FILE--
<?php

spl_autoload_register(function () { eval('namespace A { private interface B {} }'); });

class C implements \A\B {}

echo 'Should not work';

?>
--EXPECTF--
Fatal error: Class C cannot implement package private interface A\B in %s on line %d
