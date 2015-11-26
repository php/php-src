--TEST--
Test function posix_geteuid() by calling it more than or less than its expected arguments
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
echo "*** Test by calling method or function with incorrect numbers of arguments ***\n";

$extra_args = array( 12312, 2 => '1234', 'string' => 'string' );

var_dump( posix_geteuid( $extra_args ));
foreach ( $extra_args as $arg )
{
	var_dump(posix_geteuid( $arg ));
}

?>
--EXPECTF--
*** Test by calling method or function with incorrect numbers of arguments ***

Warning: posix_geteuid() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: posix_geteuid() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: posix_geteuid() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: posix_geteuid() expects exactly 0 parameters, 1 given in %s on line %d
NULL
