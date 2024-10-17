--TEST--
ReflectionProperty object default - used to say "..."
--FILE--
<?php

class C {
	public stdClass $a = FOO;
}
$reflector = new ReflectionProperty(C::class, 'a');
$lazyFactory = new ReflectionClass(C::class);
$c = $lazyFactory->newLazyGhost(function () {});

define('FOO', new stdClass);

$c->a;

var_dump( (string)$reflector );

?>
--EXPECTF--
string(%d) "Property [ public stdClass $a = object(stdClass) ]
"
