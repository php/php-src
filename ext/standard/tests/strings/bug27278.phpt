--TEST--
Bug #23894 (sprintf() decimal specifiers problem)
Bug #27278 (sprintf(): global variable changing type when passed to function by value)
--FILE--
<?php

function foo ($a)
{
	$a=sprintf("%02d",$a);
	var_dump($a);
}

$x="02";
var_dump($x);
foo($x);
var_dump($x);

?>
--EXPECT--
string(2) "02"
string(2) "02"
string(2) "02"
