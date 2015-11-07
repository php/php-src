--TEST--
Testing list() assign with default different expressions
--FILE--
<?php

list($a, $b='str') = [1];
var_dump($a, $b);


list($a, $b=new stdclass) = [1];
var_dump($a, $b);

list($a, $b=function () {return true;}) = [1];
var_dump($a, $b);


function test_func($str)
{
	var_dump($str);
	return "Yes Sir!";
}

list($a, $b=test_func("call me!")) = [1];
var_dump($a, $b);

?>
--EXPECTF--
int(1)
string(3) "str"
int(1)
object(stdClass)#1 (0) {
}
int(1)
object(Closure)#2 (0) {
}
string(8) "call me!"
int(1)
string(8) "Yes Sir!"
