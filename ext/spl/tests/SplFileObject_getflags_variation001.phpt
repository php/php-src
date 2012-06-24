--TEST--
SPL: SplFileObject::getFlags
--CREDITS--
Ricardo Oedietram <ricardo@odracir.nl>
Erwin Poeze <erwin.poeze@gmail.com>
#PFZ June PHP TestFest 2012
--FILE--
<?php

$aFlags = array(

	'DROP_NEW_LINE',
	'READ_AHEAD',
	'SKIP_EMPTY',
	'READ_CSV',
);

$fo = new SplFileObject(__FILE__);

$aResult = array();


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
