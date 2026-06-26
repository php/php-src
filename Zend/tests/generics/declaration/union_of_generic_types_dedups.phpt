--TEST--
Generics: distinct generic types that erase to the same class collapse in a union
--FILE--
<?php
class Box<T> {}
class Other<T> {}

class X {
    public null|Box<string>|Box<int>|Box<array> $y = null;
}
echo (string) (new ReflectionProperty(X::class, 'y'))->getType(), "\n";

function a(): Box<int>|Box<string> { return new Box(); }
echo (string) (new ReflectionFunction('a')->getReturnType()), "\n";

function b(): Box<int>|Other<string> { return new Box(); }
echo (string) (new ReflectionFunction('b')->getReturnType()), "\n";

function c(): Box|Box<int> { return new Box(); }
echo (string) (new ReflectionFunction('c')->getReturnType()), "\n";

function d(): Box<int>|Box { return new Box(); }
echo (string) (new ReflectionFunction('d')->getReturnType()), "\n";

interface I {}
interface J {}
function e(): (I&J)|Box<int>|Box<string> { return new Box(); }
echo (string) (new ReflectionFunction('e')->getReturnType()), "\n";
echo "ok\n";
?>
--EXPECT--
?Box
Box
Box|Other
Box
Box
(I&J)|Box
ok
