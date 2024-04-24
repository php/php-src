--TEST--
Eval Class definition should not leak memory when using compiled traits
--FILE--
<?php

trait TraitCompiled {
    public function bar(): self { return new self; }
}

const EVAL_CODE = <<<'CODE'
class A {
    use TraitCompiled;
}
CODE;

eval(EVAL_CODE);

$a1 = new A();
$a2 = $a1->bar();
var_dump($a2);

?>
DONE
--EXPECT--
object(A)#2 (0) {
}
DONE
