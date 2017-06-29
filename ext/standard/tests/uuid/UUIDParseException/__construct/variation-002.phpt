--TEST--
UUIDParseException::__construct with 3 arguments
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUIDParseException;

$e = new UUIDParseException('variation-002-reason', 'variation-002-input', 42);

foreach (['message', 'input', 'position'] as $p) {
	$p = new ReflectionProperty(UUIDParseException::class, $p);
	$p->setAccessible(true);
	var_dump($p->getValue($e));
}

?>
--EXPECT--
string(20) "variation-002-reason"
string(19) "variation-002-input"
int(42)
