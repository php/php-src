--TEST--
ReflectionMethod methods - wrong num args
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php

var_dump(new ReflectionMethod());
var_dump(new ReflectionMethod('a', 'b', 'c'));

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
Warning: ReflectionMethod::__construct() expects %s on line 3
object(ReflectionMethod)#%d (2) {
  ["name"]=>
  string(0) ""
  ["class"]=>
  string(0) ""
}

Warning: ReflectionMethod::__construct() expects %s on line 4
object(ReflectionMethod)#%d (2) {
  ["name"]=>
  string(0) ""
  ["class"]=>
  string(0) ""
}

Warning: Wrong parameter count for ReflectionMethod::isFinal() in %s on line 12
NULL

Warning: Wrong parameter count for ReflectionMethod::isAbstract() in %s on line 13
NULL

Warning: Wrong parameter count for ReflectionMethod::isPrivate() in %s on line 14
NULL

Warning: Wrong parameter count for ReflectionMethod::isProtected() in %s on line 15
NULL

Warning: Wrong parameter count for ReflectionMethod::isPublic() in %s on line 16
NULL

Warning: Wrong parameter count for ReflectionMethod::isStatic() in %s on line 17
NULL

Warning: Wrong parameter count for ReflectionMethod::isConstructor() in %s on line 18
NULL

Warning: Wrong parameter count for ReflectionMethod::isDestructor() in %s on line 19
NULL

Warning: Wrong parameter count for ReflectionMethod::getModifiers() in %s on line 20
NULL

Warning: Wrong parameter count for ReflectionFunctionAbstract::isInternal() in %s on line 21
NULL

Warning: Wrong parameter count for ReflectionFunctionAbstract::isUserDefined() in %s on line 22
NULL

Warning: Wrong parameter count for ReflectionFunctionAbstract::getFileName() in %s on line 23
NULL

Warning: Wrong parameter count for ReflectionFunctionAbstract::getStartLine() in %s on line 24
NULL

Warning: Wrong parameter count for ReflectionFunctionAbstract::getEndLine() in %s on line 25
NULL

Warning: Wrong parameter count for ReflectionFunctionAbstract::getStaticVariables() in %s on line 26
NULL

Warning: Wrong parameter count for ReflectionFunctionAbstract::getName() in %s on line 27
NULL