--TEST--
Parameter type hint - Wrong parameter for string
--FILE--
<?php

function test_str(string $a) {
	echo $a, "\n";	
}

test_str('+1.1');
test_str('-.1');
test_str('11213111112321312');
test_str('');
test_str(null);

?>
--EXPECTF--
+1.1
-.1
11213111112321312


Catchable fatal error: Argument 1 passed to test_str() must be of the type string, null given, called in %s on line %d and defined in %s on line %d
