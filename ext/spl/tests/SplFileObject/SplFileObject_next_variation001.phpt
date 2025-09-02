--TEST--
SPL: SplFileObject::next variation 001
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

$s->seek(13);
echo $s->current();

$s->next();
echo $s->current();
var_dump($s->valid());
?>
--EXPECT--
?>
bool(false)
