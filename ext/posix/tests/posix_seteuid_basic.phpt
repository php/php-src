--TEST--
Test function posix_seteuid() by calling it with its expected arguments
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--EXTENSIONS--
posix
--FILE--
<?php

$myuid = posix_geteuid();
$uid = var_dump(posix_seteuid( $myuid ) );

?>
--EXPECT--
bool(true)
