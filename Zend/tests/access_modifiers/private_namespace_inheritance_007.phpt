--TEST--
Cannot widen visibility from private(namespace) to protected in child class (soundness issue)
--FILE--
<?php

namespace App\Auth {
    class P {
        private(namespace) function x(): void {
            echo "P::x()\n";
        }
    }

    class C extends P {
        // This should be an error - widening from private(namespace) to protected
        // breaks soundness because namespace-level code can call P::x() but not C::x()
        protected function x(): void {
            echo "C::x()\n";
        }
    }

    function f(P $p): void {
        // This is legal because f() is in the same namespace as P
        $p->x();
    }

    // This would break if C::x() were allowed to be protected
    // because C::x() is protected and not accessible from f()
    f(new C());
}

?>
--EXPECTF--
Fatal error: Access level to App\Auth\C::x() must be private(namespace) (as in class App\Auth\P) or weaker in %s on line %d
