--TEST--
GH-19810: bzopen() stream mode validation
--EXTENSIONS--
bz2
--FILE--
<?php
var_dump(bzopen(STDERR, 'r'));
?>
--EXPECTF--
Warning: bzopen(): Cannot read from a stream opened in write only mode in %s on line %d
bool(false)
