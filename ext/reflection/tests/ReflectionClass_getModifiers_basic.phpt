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

?>
--EXPECT--
int(0)
int(64)
int(32)
int(0)
int(0)
int(0)
int(0)
