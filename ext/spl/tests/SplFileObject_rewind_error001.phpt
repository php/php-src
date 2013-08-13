--TEST--
SPL: SplFileObject::rewind() with a parameter. 
--CREDITS--
Ricardo Oedietram <ricardo@odracir.nl>
Erwin Poeze <erwin.poeze@gmail.com>
#PFZ June PHP TestFest 2012
--FILE--
<?php

file_put_contents('testdata.csv', 'eerste;tweede;derde');

$fo = new SplFileObject('testdata.csv');

$fo->rewind( "invalid" );

?>
--EXPECTF--
Warning: SplFileObject::rewind() expects exactly 0 parameters, 1 given in %s on line %d
