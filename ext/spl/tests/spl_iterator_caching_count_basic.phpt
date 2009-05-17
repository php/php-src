--TEST--
SPL: Caching iterator count() cache contents
--CREDITS--
Lukasz Andrzejak meltir@meltir.com
#testfest London 2009-05-09
--FILE--
<?php
$i = new ArrayIterator(array(1,1,1,1,1));
$i = new CachingIterator($i,CachingIterator::FULL_CACHE);
foreach ($i as $value) {
  echo $i->count()."\n";
}
?>
===DONE===
--EXPECT--
1
2
3
4
5
===DONE===