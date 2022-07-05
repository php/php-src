--TEST--
SPL: SplFileObject::key error
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
$s->next();
var_dump($s->key());
var_dump($s->valid());
?>
--EXPECT--
int(14)
bool(false)
