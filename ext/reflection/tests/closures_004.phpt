--TEST--
Reflection on closures: Segfault with getClosure() on closure itself
--FILE--
<?php
$closure = function() { echo "Invoked!\n"; };

$method = new ReflectionFunction ($closure);

$closure2 = $method->getClosure ();

$closure2 ();
$closure2->__invoke ();

unset ($closure);

$closure2 ();
$closure2->__invoke ();

$closure = function() { echo "Invoked!\n"; };

$method = new ReflectionMethod ($closure, '__invoke');
$closure2 = $method->getClosure ($closure);

$closure2 ();
$closure2->__invoke ();

unset ($closure);

$closure2 ();
$closure2->__invoke ();

?>
--EXPECT--
Invoked!
Invoked!
Invoked!
Invoked!
Invoked!
Invoked!
Invoked!
Invoked!
