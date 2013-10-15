--TEST--
SPL: SplFileObject::rewind() with a parameter. 
--CREDITS--
Ricardo Oedietram <ricardo@odracir.nl>
Erwin Poeze <erwin.poeze@gmail.com>
#PFZ June PHP TestFest 2012
--FILE--
<?php

file_put_contents('SplFileObject_rewind_error001.csv', 'eerste;tweede;derde');

$fo = new SplFileObject('SplFileObject_rewind_error001.csv');

$fo->rewind( "invalid" );

?>
--CLEAN--
<?php
unlink('SplFileObject_rewind_error001.csv');
?>
--EXPECTF--
Warning: SplFileObject::rewind() expects exactly 0 parameters, 1 given in %s on line %d
