--TEST--
func_get_arg test
--FILE--
<?php

function foo($a)
{
   $a=5;
   echo func_get_arg(-1);
   echo func_get_arg(2);
}
foo(2);
echo "\n";
?>
--EXPECTF--
Warning: func_get_arg():  The argument number should be >= 0 in %s on line %d

Warning: func_get_arg():  Argument 2 not passed to function in %s on line %d
