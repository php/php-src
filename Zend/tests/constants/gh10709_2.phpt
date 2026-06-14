--TEST--
GH-10709: Recursive class constant evaluation
--FILE--
<?php

class B {
    public $prop = A::C;
}

spl_autoload_register(function ($class) {
    class A { const C = "A"; }
    var_dump(new B());
});

try {
    var_dump(new B());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(B)#2 (1) {
  ["prop"]=>
  string(1) "A"
}
object(B)#2 (1) {
  ["prop"]=>
  string(1) "A"
}
