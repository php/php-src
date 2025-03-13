--TEST--
reflection on inner classes
--FILE--
<?php

namespace n\s;

class Outer {
    class Middle {
        class Inner {}
    }
}

$outer = new \ReflectionClass(Outer::class);
$ref = new \ReflectionClass('n\s\Outer:>Middle:>Inner');
var_dump($ref->getName());
var_dump($ref->getShortName());
var_dump($ref->isInnerClass());
var_dump($outer->isInnerClass());
var_dump($ref->isPrivate());
var_dump($ref->isProtected());
var_dump($ref->isPublic());
?>
--EXPECT--
string(24) "n\s\Outer:>Middle:>Inner"
string(20) "Outer:>Middle:>Inner"
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
