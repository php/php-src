--TEST--
ReflectionMethod methods - wrong num args
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php

new ReflectionMethod();
new ReflectionMethod('a', 'b', 'c');


class C {
    public function f() {}
}

$rm = new ReflectionMethod('C', 'f');

var_dump($rm->isFinal(1));
var_dump($rm->isAbstract(1));
var_dump($rm->isPrivate(1));
var_dump($rm->isProtected(1));
var_dump($rm->isPublic(1));
var_dump($rm->isStatic(1));
var_dump($rm->isConstructor(1));
var_dump($rm->isDestructor(1));
var_dump($rm->getModifiers(1));
var_dump($rm->isInternal(1));
var_dump($rm->isUserDefined(1));
var_dump($rm->getFileName(1));
var_dump($rm->getStartLine(1));
var_dump($rm->getEndLine(1));
var_dump($rm->getStaticVariables(1));
var_dump($rm->getName(1));


?>
--EXPECTF--
Warning: Wrong parameter count for ReflectionMethod::__construct() in %s on line %d

Warning: Wrong parameter count for ReflectionMethod::__construct() in %s on line %d

Warning: ReflectionMethod::isFinal() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionMethod::isAbstract() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionMethod::isPrivate() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionMethod::isProtected() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionMethod::isPublic() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionMethod::isStatic() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionMethod::isConstructor() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionMethod::isDestructor() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionMethod::getModifiers() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionFunctionAbstract::isInternal() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionFunctionAbstract::isUserDefined() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionFunctionAbstract::getFileName() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionFunctionAbstract::getStartLine() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionFunctionAbstract::getEndLine() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionFunctionAbstract::getStaticVariables() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionFunctionAbstract::getName() expects exactly 0 parameters, 1 given in %s on line %d
NULL
