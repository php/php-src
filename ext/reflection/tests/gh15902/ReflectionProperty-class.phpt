--TEST--
ReflectionProperty object default - used to say "__CLASS__"
--INI--
opcache.enable_cli=0
--FILE--
<?php

class C {
	public stdClass $a = FOO;
}
$reflector = new ReflectionProperty(C::class, 'a');

define('FOO', new stdClass);
new C;

echo $reflector;

?>
--EXPECTF--
Property [ public stdClass $a = object(stdClass) ]
