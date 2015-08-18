--TEST--
func_get_arg test (PHP7)
--FILE--
<?php

function foo($a)
{
   $a=5;
   echo func_get_arg(0);
}
foo(2);
echo "\n";
?>
--EXPECT--
5
