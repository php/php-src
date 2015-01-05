--TEST--
Test function proc_nice() by calling it more than or less than its expected arguments
--SKIPIF--
<?php
if(!function_exists('proc_nice')) die("skip. proc_nice not available ");
?>
--FILE--
<?php
echo "*** Test by calling method or function with incorrect numbers of arguments ***\n";

$priority = 1;

$extra_arg = 1;

var_dump(proc_nice( $priority, $extra_arg) );

var_dump(proc_nice(  ) );


?>
--EXPECTF--
*** Test by calling method or function with incorrect numbers of arguments ***

Warning: proc_nice() expects exactly 1 parameter, 2 given in %s line %d
bool(false)

Warning: proc_nice() expects exactly 1 parameter, 0 given in %s line %d
bool(false)
