--TEST--
Parameter type hint - Testing default parameter value
--FILE--
<?php

function test(string &$a, integer $x = 1) {
	var_dump($x);
}

$y = 'foo';
test($y, 2);
test($y);

?>
--EXPECT--
int(2)
int(1)
