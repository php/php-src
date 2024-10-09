--TEST--
ReflectionProperty object default - used to say "__CLASS__"
--FILE--
<?php

class C {
	public stdClass $a = FOO;
}
$reflector = new ReflectionProperty(C::class, 'a');

define('FOO', new stdClass);
new C;

var_dump( (string)$reflector );

?>
--EXPECTF--
string(%d) "Property [ public stdClass $a = object(stdClass) ]
"
