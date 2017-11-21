--TEST--
Arbitrary expression interpolation
--FILE--
<?php

function a() {return "d";}
$value = 10;

var_dump(
	"Result: #{$value * 5}",
	"abc#{(function(){return "def";})()}",
	"abc#{a()}",
	"abc#{a()}#{a()}",
	"ab \#{c}",
	`echo #{$value * 5}`,
	<<<END
Result: #{$value * 5}
abc#{(function(){return "def";})()}
abc#{a()}
ab \#{c}
END
);
?>
--EXPECT--	
string(10) "Result: 50"
string(6) "abcdef"
string(4) "abcd"
string(5) "abcdd"
string(7) "ab #{c}"
string(3) "50
"
string(30) "Result: 50
abcdef
abcd
ab #{c}"