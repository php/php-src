--TEST--
Reflection on closures: Segfault with getClosure() on closure itself
--FILE-- 
<?php

$closure = function() { echo "Invoked!\n"; };

$method = new ReflectionMethod ($closure);

$closure2 = $method->getClosure ($closure);

$closure2 ();
$closure2->__invoke ();

unset ($closure);

$closure2 ();
$closure2->__invoke ();

?>
===DONE===
--EXPECTF--
Invoked!
Invoked!
Invoked!
Invoked!
===DONE===
