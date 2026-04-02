--TEST--
Eval code should not leak memory when using traits
--FILE--
<?php

const EVAL_CODE = <<<'CODE'
trait TraitEval {
    public self $foo;
    public function bar(): self { return new self; }
}
CODE;

eval(EVAL_CODE);

class A {
    use TraitEval;
}

$a1 = new A();
$a2 = $a1->bar();
var_dump($a2);

$methodInfo = ReflectionMethod::createFromMethodName("A::bar");
var_dump($methodInfo->getReturnType()->getName());
$property = new ReflectionProperty('A', 'foo');
var_dump($property->getType()->getName());

$methodInfo = ReflectionMethod::createFromMethodName("TraitEval::bar");
var_dump($methodInfo->getReturnType()->getName());
$property = new ReflectionProperty('TraitEval', 'foo');
var_dump($property->getType()->getName());

?>
DONE
--EXPECT--
object(A)#2 (0) {
  ["foo"]=>
  uninitialized(A)
}
string(1) "A"
string(1) "A"
string(4) "self"
string(4) "self"
DONE
