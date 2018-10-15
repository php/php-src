--TEST--
Test function posix_setgid() by calling it with its expected arguments
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php


echo "*** Test by calling method or function with its expected arguments ***\n";

$gid = posix_getgid();
var_dump(posix_setgid( $gid ) );


?>
===DONE===
--EXPECTF--
*** Test by calling method or function with its expected arguments ***
bool(true)
===DONE===
	
