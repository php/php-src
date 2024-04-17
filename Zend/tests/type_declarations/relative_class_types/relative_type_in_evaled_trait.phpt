--TEST--
Eval code should not leak memory when using traits
--FILE--
<?php

const EVAL_CODE = <<<'CODE'
trait TraitExample {
    public function bar(): self|C|D { return new self; }
}
class B {
    use TraitExample;
}
CODE;

eval(EVAL_CODE);

class A {
    use TraitExample;
}

$a1 = new A();
$a2 = $a1->bar();
var_dump($a2);

//$b1 = new B();
//$b2 = $b1->bar();
//var_dump($b2);

?>
DONE
--EXPECT--
object(A)#2 (0) {
}
DONE
