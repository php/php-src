--TEST--
Arbitrary expression interpolation
--FILE--
<?php

function a() {return "d";}
$value = 10;

var_dump(
	"Result: #{$value * 5}",
	"abc#{(function(){return "def";})()}",
	"abc#{a()}"
);
?>
--EXPECT--	
string(10) "Result: 50"
string(6) "abcdef"
string(6) "abcd"
