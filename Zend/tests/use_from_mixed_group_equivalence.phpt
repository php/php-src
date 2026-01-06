--TEST--
use-from mixed-group equivalence
--FILE--
<?php
namespace P {
    class A {
        public static function who() {
            echo "A\n";
        }
    }

    namespace B {
        function fn() {
            echo "B::fn\n";
        }
    }
}

namespace TestFrom {
    use {A, function B\fn} from P;
    A::who();
    fn();
}

echo "----\n";

namespace TestPrefix {
    use P\{A, function B\fn};
    A::who();
    fn();
}

?>
--EXPECT--
A
B::fn
----
A
B::fn
