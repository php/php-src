--TEST--
SPL: SplFileObject::getFlags
--CREDITS--
Ricardo Oedietram <ricardo@odracir.nl>
Erwin Poeze <erwin.poeze@gmail.com>
#PFZ June PHP TestFest 2012
--FILE--
<?php

$fo = new SplFileObject(__FILE__);

$fo->setFlags(SplFileObject::DROP_NEW_LINE);
var_dump($fo->getFlags());

$fo->setFlags(SplFileObject::READ_AHEAD);
var_dump($fo->getFlags());

$fo->setFlags(SplFileObject::SKIP_EMPTY);
var_dump($fo->getFlags());

$fo->setFlags(SplFileObject::READ_CSV);
var_dump($fo->getFlags());

?>
--EXPECT--
int(1)
int(2)
int(4)
int(8)
