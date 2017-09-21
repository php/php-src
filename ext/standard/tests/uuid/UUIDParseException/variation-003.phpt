--TEST--
UUIDParseException construction with custom position and previous exception
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$e = new UUIDParseException('variation-003-reason', 'variation-003-input', 3, $previous = new Exception);

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
string(19) "variation-003-input"
int(3)
string(20) "variation-003-reason"
int(3)
bool(true)
