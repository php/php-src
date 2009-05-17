--TEST--
SPL: IteratorIterator foreach by reference failure
--CREDITS--
Lukasz Andrzejak meltir@meltir.com
#testfest London 2009-05-09
--FILE--
<?php
$i = new ArrayIterator(array(1,1,1,1,1));
$iii = new IteratorIterator($i);
p($iii);
function p ($i) {
  foreach ($i as &$value) {}
}
?>
--EXPECTF--
Fatal error: An iterator cannot be used with foreach by reference in %s