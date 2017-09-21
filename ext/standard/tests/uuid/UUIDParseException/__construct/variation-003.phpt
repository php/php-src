--TEST--
UUIDParseException::__construct with 4 arguments
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$e = new UUIDParseException('variation-003-reason', 'variation-003-input', 84, $previous = new Exception);

foreach (['message', 'input', 'position'] as $p) {
	$p = new ReflectionProperty(UUIDParseException::class, $p);
	$p->setAccessible(true);
	var_dump($p->getValue($e));
}

$p = new ReflectionProperty(Exception::class, 'previous');
$p->setAccessible(true);
var_dump($p->getValue($e) === $previous);

?>
--EXPECT--
string(20) "variation-003-reason"
string(19) "variation-003-input"
int(84)
bool(true)
