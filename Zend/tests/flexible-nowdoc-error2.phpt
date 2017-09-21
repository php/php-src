--TEST--
Flexible nowdoc syntax 2: mixing spaces and tabs in body
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