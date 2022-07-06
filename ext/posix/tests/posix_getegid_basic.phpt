--TEST--
Test function posix_getegid() by calling it with its expected arguments
--CREDITS--
Michelangelo van Dam dragonbe@gmail.com
#PHPTestFest Dutch PHP Conference 2012
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
--FILE--
<?php
var_dump(posix_getegid());
?>
--EXPECTF--
int(%d)
