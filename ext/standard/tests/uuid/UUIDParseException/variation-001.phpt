--TEST--
UUIDParseException construction with custom position
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$e = new PHP\Std\UUIDParseException('variation-001-reason', 'variation-001-input', 1);

var_dump(
	$e->getCode(),
	$e->getFile() === __FILE__,
	$e->getInput(),
	$e->getLine(),
	$e->getMessage(),
	$e->getPosition(),
	$e->getPrevious()
);

?>
--EXPECT--
int(0)
bool(true)
string(19) "variation-001-input"
int(3)
string(20) "variation-001-reason"
int(1)
NULL
