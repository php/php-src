--TEST--
Relative class types from traits MUST NOT be resolved when used inside a trait
--FILE--
<?php

trait TraitCompiled {
    public self $foo;
    public function bar(): self { return new self; }
}

trait SecondTrait {
    use TraitCompiled;
}

$methodInfo = ReflectionMethod::createFromMethodName("TraitCompiled::bar");
var_dump($methodInfo->getReturnType()->getName());
$property = new ReflectionProperty('TraitCompiled', 'foo');
var_dump($property->getType()->getName());

$methodInfo = ReflectionMethod::createFromMethodName("SecondTrait::bar");
var_dump($methodInfo->getReturnType()->getName());
$property = new ReflectionProperty('SecondTrait', 'foo');
var_dump($property->getType()->getName());

?>
DONE
--EXPECT--
string(4) "self"
string(4) "self"
string(4) "self"
string(4) "self"
DONE
