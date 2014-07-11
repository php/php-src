--TEST--
SPL: SplFileObject::getFlags error 001
--CREDITS--
Erwin Poeze <erwin.poeze@gmail.com>
--FILE--
<?php

file_put_contents('SplFileObject_getflags_error002.csv', 'eerste;tweede;derde');

$fo = new SplFileObject('SplFileObject_getflags_error002.csv');
$fo->setFlags(SplFileObject::READ_CSV);

$fo->getFlags('fake');

?>
--CLEAN--
<?php
unlink('SplFileObject_getflags_error002.csv');
?>
--EXPECTF--
Warning: SplFileObject::getFlags() expects exactly 0 parameters, 1 given in %s on line %d
