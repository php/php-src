--TEST--
Parameter type hint - Testing string type hint with reference
--FILE--
<?php

function test(string &$a) {
	$a .= '!!!';
}

$x = 'foo';

test($x);

var_dump($x);

?>
--EXPECT--
string(6) "foo!!!"
--UEXPECT--
unicode(6) "foo!!!"
