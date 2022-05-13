--TEST--
Test function posix_setgid() by calling it with its expected arguments
--EXTENSIONS--
posix
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
--EXPECT--
*** Test by calling method or function with its expected arguments ***
bool(true)
	
