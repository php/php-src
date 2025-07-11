--TEST--
Pipe operator supports built-in functions
--FILE--
<?php

$res1 = "Hello" |> 'strlen';

var_dump($res1);
?>
--EXPECT--
int(5)
