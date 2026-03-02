--TEST--
Bug #75673 (SplStack::unserialize() behavior)
--FILE--
<?php
$stack = new SplStack();
$stack->push("one");
$stack->push("two");

$serialized = $stack->serialize();
var_dump($stack->count());
$stack->unserialize($serialized);
var_dump($stack->count());
$stack->unserialize($serialized);
var_dump($stack->count());
?>
--EXPECT--
int(2)
int(2)
int(2)
