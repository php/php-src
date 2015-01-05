--TEST--
SPL: Caching iterator count() cache failure
--CREDITS--
Lukasz Andrzejak meltir@meltir.com
#testfest London 2009-05-09
--FILE--
<?php
$i = new ArrayIterator(array(1,1,1,1,1));
$i = new CachingIterator($i);
try {
  $i->count();
  echo "Should have caused an exception";
} catch (BadMethodCallException $e) {
  echo "Exception raised\n";
}

?>
===DONE===
--EXPECT--
Exception raised
===DONE===