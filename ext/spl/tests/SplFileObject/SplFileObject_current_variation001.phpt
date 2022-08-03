--TEST--
SPL: SplFileObject::current variation
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

echo $s->current();
echo $s->current();
?>
--EXPECT--
//line 3
//line 3
