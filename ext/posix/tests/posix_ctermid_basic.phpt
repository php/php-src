--TEST--
Test function posix_ctermid() by calling it with its expected arguments
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--EXTENSIONS--
posix
--FILE--
<?php

var_dump( posix_ctermid() );

?>
--EXPECTF--
string(%d) %s
