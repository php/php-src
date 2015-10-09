--TEST--
Bug #70681: Segfault when binding $this of internal instance method to null
--FILE--
<?php

$c = (new ReflectionMethod('SplStack', 'count'))->getClosure(new SplStack);
$c = $c->bindTo(null);

?>
--EXPECTF--
Warning: Cannot unbind $this of internal method in %s on line %d
