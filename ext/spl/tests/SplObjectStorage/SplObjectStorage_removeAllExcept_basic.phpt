--TEST--
Check that SplObjectStorage::removeUncommon functions when receiving proper input
--CREDITS--
Matthew Turland (me@matthewturland.com)
--FILE--
<?php

$a = (object) 'a';
$b = (object) 'b';
$c = (object) 'c';

$foo = new SplObjectStorage;
$foo->offsetSet($a);
$foo->offsetSet($b);

$bar = new SplObjectStorage;
$bar->offsetSet($b);
$bar->offsetSet($c);

$foo->removeAllExcept($bar);
var_dump($foo->offsetExists($a));
var_dump($foo->offsetExists($b));

?>
--EXPECT--
bool(false)
bool(true)
