--TEST--
ReflectionType leak
--FILE--
<?php

$closure = function(Test $x): Test2 { return new Test2($x); };
$rm = new ReflectionMethod($closure, '__invoke');
$rp = $rm->getParameters()[0];
$rt = $rp->getType();
$rrt = $rm->getReturnType();
unset($rm, $rp);
var_dump((string) $rt, (string) $rrt);

--EXPECT--
string(4) "Test"
string(5) "Test2"

