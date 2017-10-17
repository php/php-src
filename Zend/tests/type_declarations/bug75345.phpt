--TEST--
Bug #75345 (Strict type declarations not enforced for Reflection API invocation)
--FILE--
<?php
declare(strict_types=1);

function f(bool $a)
{
    var_dump($a);
}

$f = new ReflectionFunction('f');
$f->invoke(1);
--EXPECTF--	
Fatal error: Uncaught TypeError: Argument 1 passed to f() must be of the type boolean, integer given in %s
Stack trace:
#0 [internal function]: f(1)
#1 %s ReflectionFunction->invoke(1)
#2 {main}
  thrown in %s
