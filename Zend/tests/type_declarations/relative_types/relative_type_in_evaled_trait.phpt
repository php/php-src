--TEST--
Eval code should not leak memory when using traits
--FILE--
<?php

const EVAL_CODE = <<<'CODE'
trait TraitEval {
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

?>
DONE
--EXPECT--
object(A)#2 (0) {
}
DONE
