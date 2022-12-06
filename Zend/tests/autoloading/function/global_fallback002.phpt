--TEST--
Fallback to global function should not trigger autoloading.
--FILE--
<?php

namespace {
    function loader($name) {
        echo "function loader called with $name\n";
    }

    autoload_register_function('loader');

    function foo() {
        echo "I am foo in global namespace.\n";
    }
}

namespace bar {
	foo();
}

?>
--EXPECT--
I am foo in global namespace.
