--TEST--
SPL: SplFileObject::current variation error
--CREDITS--
Ricardo Oedietram <ricardo@odracir.nl>
Erwin Poeze <erwin.poeze@gmail.com>
#PFZ June PHP TestFest 2012
--FILE--
<?php
//line 2
//line 3
//line 4
//line 5
$s = new SplFileObject(__FILE__);
$s->seek(2);

echo $s->current('foo');
?>
--EXPECTF--
Warning: SplFileObject::current() expects exactly 0 parameters, 1 given in %s on line %d
