--TEST--
SPL: SplFileObject::getFlags error 001
--CREDITS--
Ricardo Oedietram <ricardo@odracir.nl>
Erwin Poeze <erwin.poeze@gmail.com>
#PFZ June PHP TestFest 2012
--FILE--
<?php

file_put_contents('SplFileObject_getflags_error001.csv', 'eerste;tweede;derde');


$fo = new SplFileObject('SplFileObject_getflags_error001.csv');
$fo->setFlags(SplFileObject::READ_CSV);

$fo->setFlags(SplFileObject::DROP_NEW_LINE);

var_dump($fo->getFlags());

?>
--CLEAN--
<?php
unlink('SplFileObject_getflags_error001.csv');
?>
--EXPECT--
int(1)
