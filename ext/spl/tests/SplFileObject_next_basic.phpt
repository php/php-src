--TEST--
SPL: SplFileObject::next basic
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
echo $s->current();
$s->next();

echo $s->current();

?>
--EXPECT--
<?php
//line 2
