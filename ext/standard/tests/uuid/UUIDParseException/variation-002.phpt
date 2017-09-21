--TEST--
UUIDParseException construction with custom position and previous error
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$e = new UUIDParseException('variation-002-reason', 'variation-002-input', 2, $previous = new Error);

var_dump(
	$e->getCode(),
	$e->getFile() === __FILE__,
	$e->getInput(),
	$e->getLine(),
	$e->getMessage(),
	$e->getPosition(),
	$e->getPrevious() === $previous
);

?>
--EXPECT--
int(0)
bool(true)
string(19) "variation-002-input"
int(3)
string(20) "variation-002-reason"
int(2)
bool(true)
