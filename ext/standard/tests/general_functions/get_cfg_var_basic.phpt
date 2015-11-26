--TEST--
Test function get_cfg_var() by calling it with its expected arguments
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
session.use_cookies=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php


echo "*** Test by calling method or function with its expected arguments ***\n";
var_dump(get_cfg_var( 'session.use_cookies' ) );
var_dump(get_cfg_var( 'session.serialize_handler' ) );
var_dump(get_cfg_var( 'session.save_handler' ) );

?>
--EXPECTF--
*** Test by calling method or function with its expected arguments ***
string(1) "0"
string(3) "php"
string(5) "files"
