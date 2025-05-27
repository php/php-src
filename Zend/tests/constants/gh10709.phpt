--TEST--
GH-10709: Recursive class constant evaluation
--FILE--
<?php

class B { const C = A::C . "B"; }

spl_autoload_register(function ($class) {
    class A { const C = "A"; }
    var_dump(B::C);
});

try {
    new B();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(2) "AB"
