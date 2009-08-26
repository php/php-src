--TEST--
Test function proc_nice() by calling it more than or less than its expected arguments
--FILE--
<?php


echo "*** Test by calling method or function with incorrect numbers of arguments ***\n"

$priority = 


$extra_arg = 

var_dump(proc_nice( $priority, $extra_arg ) );

var_dump(proc_nice(  ) );


?>
--EXPECTF--
Parse error: syntax error, unexpected T_VARIABLE, expecting ',' or ';' in %s on line %d
