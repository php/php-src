--TEST--
Flexible nowdoc syntax 1: different indentation for body (spaces) ending marker (tabs)
--FILE--
<?php

try {
	echo <<<'END'
	   a
	  b
	 c
		END;
} catch (ParseError $pe) {
	var_dump($e->getMessage());
}
--EXPECTF--
Parse error: Invalid indentation - tabs and spaces cannot be mixed in %s on line %d