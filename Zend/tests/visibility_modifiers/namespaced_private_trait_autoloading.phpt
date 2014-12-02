--TEST--
Visibility modifiers: Namespaced private trait, autoloading
--FILE--
<?php

spl_autoload_register(function () { eval('namespace A { private trait B {} }'); });

class C
{
    use \A\B;
}

echo 'Should not work';

?>
--EXPECTF--
Fatal error: Class C cannot use package private trait A\B in %s on line %d
