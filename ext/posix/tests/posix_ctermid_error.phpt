--TEST--
Test function posix_ctermid() by calling it more than or less than its expected arguments
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

var_dump( posix_ctermid( 'foo' ) );

?>
--EXPECTF--
Warning: Wrong parameter count for posix_ctermid() in %s.php on line %d
NULL
