--TEST--
ReflectionClass::getModifiers()
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php

class a {}
abstract class b {}
final class c {}
interface d {}
class e implements d {}
interface f extends d {}
class g extends b {}
private interface h {}
private class i extends g implements h {}
private trait j {}

function dump_modifiers($class) {
	$obj = new ReflectionClass($class);
	var_dump($obj->getModifiers());
}

dump_modifiers('a');
dump_modifiers('b');
dump_modifiers('c');
dump_modifiers('d');
dump_modifiers('e');
dump_modifiers('f');
dump_modifiers('g');
dump_modifiers('h');
dump_modifiers('i');
dump_modifiers('j');

?>
--EXPECT--
int(256)
int(288)
int(260)
int(320)
int(524544)
int(524608)
int(256)
int(1088)
int(525312)
int(1152)
