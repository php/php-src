--TEST--
ReflectionProperty object default - used to say "callable"
--INI--
opcache.enable_cli=0
--FILE--
<?php

class C {
	public stdClass $a = FOO;
}
define('FOO', new stdClass);

new C;

$reflector = new ReflectionProperty(C::class, 'a');
echo $reflector;

?>
--EXPECTF--
Property [ public stdClass $a = object(stdClass) ]
