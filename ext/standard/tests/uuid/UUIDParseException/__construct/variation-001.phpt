--TEST--
UUIDParseException::__construct with 2 arguments
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$e = new UUIDParseException('variation-001-reason', 'variation-001-input');

foreach (['message', 'input', 'position'] as $p) {
	$p = new ReflectionProperty(UUIDParseException::class, $p);
	$p->setAccessible(true);
	var_dump($p->getValue($e));
}

?>
--EXPECT--
string(20) "variation-001-reason"
string(19) "variation-001-input"
int(0)
