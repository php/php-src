--TEST--
Test function posix_ttyname() by calling it more than or less than its expected arguments
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded('posix')) {
    die('SKIP The posix extension is not loaded.');
}
?>
--FILE--
<?php


echo "*** Test by calling method or function with incorrect numbers of arguments ***\n";

$fd = 'foo';
$extra_arg = 'bar';

var_dump(posix_ttyname( $fd, $extra_arg ) );

var_dump(posix_ttyname(  ) );


?>
--EXPECTF--
*** Test by calling method or function with incorrect numbers of arguments ***

Warning: posix_ttyname() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

Warning: posix_ttyname() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
