--TEST--
GH-10709: Recursive class constant evaluation with outer call failing
--FILE--
<?php

class S {
    public function __toString() {
        static $i = 0;
        $i++;
        if ($i === 1) {
            return 'S';
        } else {
            throw new \Exception('Thrown from S');
        }
    }
}

const S = new S();

class B {
    public $prop = A::C . S;
}

spl_autoload_register(function ($class) {
    class A { const C = "A"; }
    var_dump(new B());
});

var_dump(new B());

?>
--EXPECTF--
object(B)#3 (1) {
  ["prop"]=>
  string(2) "AS"
}

Fatal error: Uncaught Exception: Thrown from S in %s:%d
Stack trace:
#0 %s(%d): [constant expression]()
#1 %s(%d): S->__toString()
#2 {main}
  thrown in %s on line %d
