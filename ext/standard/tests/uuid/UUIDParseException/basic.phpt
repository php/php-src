--TEST--
UUIDParseException construction
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$e = new PHP\Std\UUIDParseException('basic-reason', 'basic-input');

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
string(11) "basic-input"
int(3)
string(12) "basic-reason"
int(0)
NULL
