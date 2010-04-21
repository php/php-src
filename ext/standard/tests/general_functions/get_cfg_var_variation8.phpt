--TEST--
Test function get_cfg_var() by calling deprecated option
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
safe_mode=1
--SKIPIF--
<?php if (version_compare(PHP_VERSION, "5.3", "<")) die("skip requires 5.3 or greater"); ?>
--FILE--
<?php
echo "*** Test by calling method or function with deprecated option ***\n";
var_dump(get_cfg_var( 'safe_mode' ) );

?>
--EXPECTF--
Warning: Directive 'safe_mode' is deprecated in PHP 5.3 and greater in %s on line 0
*** Test by calling method or function with deprecated option ***
string(1) "1"

