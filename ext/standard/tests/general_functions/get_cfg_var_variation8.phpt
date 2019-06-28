--TEST--
Test function get_cfg_var() by calling deprecated option
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
magic_quotes_gpc=1
--SKIPIF--
<?php if (getenv('SKIP_ASAN')) die('xfail Startup failure leak'); ?>
--FILE--
<?php
echo "*** Test by calling method or function with deprecated option ***\n";
var_dump(get_cfg_var( 'magic_quotes_gpc' ) );

?>
--EXPECT--
Fatal error: Directive 'magic_quotes_gpc' is no longer available in PHP in Unknown on line 0
