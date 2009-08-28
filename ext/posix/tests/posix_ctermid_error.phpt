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
Warning: posix_ctermid() expects exactly 0 parameters, 1 given in %s on line %d
NULL
