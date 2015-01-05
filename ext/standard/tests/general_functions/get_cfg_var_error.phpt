--TEST--
Test function get_cfg_var() by calling it more than or less than its expected arguments
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
session.use_cookies=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php

echo "*** Test by calling method or function with incorrect numbers of arguments ***\n";

var_dump(get_cfg_var( 'session.use_cookies', 'session.serialize_handler' ) );
var_dump(get_cfg_var(  ) );


?>
--EXPECTF--
*** Test by calling method or function with incorrect numbers of arguments ***

Warning: get_cfg_var() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: get_cfg_var() expects exactly 1 parameter, 0 given in %s on line %d
NULL
