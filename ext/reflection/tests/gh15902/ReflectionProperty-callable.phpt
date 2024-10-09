--TEST--
ReflectionProperty object default - used to say "callable"
--FILE--
<?php

class C {
	public stdClass $a = FOO;
}
define('FOO', new stdClass);

new C;

$reflector = new ReflectionProperty(C::class, 'a');
var_dump( (string)$reflector );

?>
--EXPECTF--
string(%d) "Property [ public stdClass $a = object(stdClass) ]
"
