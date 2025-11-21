--TEST--
Cannot change from protected to private(namespace) - different axes (soundness issue)
--FILE--
<?php

namespace App\Auth {
    class P {
        protected function x(): void {
            echo "P::x()\n";
        }
    }

    class C extends P {
        // This should be an error - changing from inheritance axis to namespace axis
        // breaks soundness because child classes outside this namespace expect
        // to access the method via protected inheritance
        private(namespace) function x(): void {
            echo "C::x()\n";
        }
    }
}

?>
--EXPECTF--
Fatal error: Access level to App\Auth\C::x() must be protected (as in class App\Auth\P) or weaker in %s on line %d
